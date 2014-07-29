//
//  MasterViewController.m
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/25/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#import "MasterViewController.h"
#include "wa_api_error_codes.h"
#include "wa_utils_string.h"
#include "OptionWindowController.h"

@interface MasterViewController ()
@property (weak) IBOutlet NSTableView *processTableView;
@property (weak) IBOutlet NSTextField *textProcessListed;
@property (weak) IBOutlet NSTextField *textProcessScanned;
@property (weak) IBOutlet NSTableView *detailTableView;
@property (weak) IBOutlet NSTextField *textEngineUsed;
- (IBAction)clickOption:(id)sender;

@end

@implementation MasterViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        // Initialization code here.
        [self initialize];
        [self setupProcessReceiver];
    }
    return self;
}

/*- ( void ) loadView
{
    [super loadView];
    //[self setupDetailTable];
    [self initialize];
    [self setupProcessReceiver];
}*/

- ( void ) initialize
{
    self->m_pSignal = new TSignal;
    self->m_pLister = nullptr;
}

- ( void ) destroy
{
    self->m_pLister->stop();
    if ( self->m_pLister != nullptr )
    {
        delete self->m_pLister;
        self->m_pLister = nullptr;
    }
    
    if ( self->m_pSignal != nullptr )
    {
        CFRelease( self->m_pSignal->cfSource );
        delete self->m_pSignal;
        self->m_pSignal = nullptr;
    }
}

- ( void ) setupProcessReceiver
{   
    CFRunLoopSourceContext context = {0, (__bridge  void* )self, NULL, NULL, NULL, NULL, NULL, &receiverSchedule, NULL, &receiverPerform };
    CFRunLoopSourceRef cf_source = CFRunLoopSourceCreate( kCFAllocatorDefault, 0, &context );
    CFRunLoopRef cf_runner = [[NSRunLoop currentRunLoop] getCFRunLoop];

    CFRunLoopAddSource( cf_runner, cf_source, kCFRunLoopCommonModes );
    [self setSource: cf_source
          andRunner: cf_runner];
}

- ( void ) setSource: ( CFRunLoopSourceRef ) cfSource andRunner: ( CFRunLoopRef ) runner
{
    self->m_pSignal->cfSource = cfSource;
    self->m_pSignal->cfRunner = runner;
}

- ( void ) setupProcessLister
{
    if ( self->m_pLister == nullptr )
    {
        self->m_pLister = new WaProcessLister();
        self->m_pLister->start( self->m_pSignal );
    }
}


- ( void ) updateView
{
    [self.textProcessListed setStringValue: [NSString stringWithFormat: @"Processes listed: %d", ( int )self->m_pSignal->processList.size()]];
    
    [self.textProcessScanned setStringValue:[NSString stringWithFormat: @"Processes scanned: %d", [ self countScanned ] ] ];

    [self.processTableView reloadData];
}

- ( int ) countScanned
{
    int count = 0;
    wa_process_list& list = self->m_pSignal->processList;
    for ( int i = 0 ; i < list.size() ; i++ )
    {
        if ( list[i].scan_detail->scan_status != "scanning" )
            count ++;
    }
    return count;
}

- ( void ) updateDetailTable: ( NSInteger ) selectionId
{
    if ( selectionId >= 0 )
    {
        TScanDetail* p_detail = self->m_pSignal->processList[( int )selectionId].scan_detail;
        
        [self.textEngineUsed setStringValue: [NSString stringWithFormat: @"Engine used: %d", p_detail->n_engine]];
        
        if ( [self fillDetailTitle: p_detail->p_result] == TRUE )
            [self.detailTableView reloadData];
    }
}


- ( BOOL ) fillDetailTitle: ( WaJson* ) p_detail
{
    WaJson result_json;
    if ( ( p_detail == nullptr ) ||
        ( WAAPI_FAILED( p_detail->get( L"scan_results", result_json ) ) ) ||
        ( WAAPI_FAILED( result_json.get( L"scan_details", self->m_selJson ) ) ) )
    {
        return FALSE;
    }
    
    wa_json_object_keys a_keys = self->m_selJson.keys();
    NSArray* p_table_columns = self.detailTableView.tableColumns;
    NSUInteger column_id = 1;
    
    for( auto iter = a_keys.begin() ; iter != a_keys.end() ; iter ++, column_id++ )
    {
        std::string s_tile = WaStringUtils::wstrToStr( *iter );
        NSString* title = [NSString stringWithFormat: @"%s", s_tile.c_str()];
        NSTableColumn* p_column = [p_table_columns objectAtIndex: column_id];        
        if ( [[[p_column headerCell] stringValue] isEqualToString: title] == FALSE )
        {
            [[p_column headerCell] setStringValue: title];
            //[[p_column dataCell] setAlignment: NSCenterTextAlignment];
        }
        /*if ( column_id < n_column )
        {
            NSTableColumn* p_column = [p_table_columns objectAtIndex: column_id];
            
            if ( [[[p_column headerCell] stringValue] isEqualToString: title] == FALSE )
            {
                [[p_column headerCell] setStringValue: title];
            }
        }
        else
        {
            NSTableColumn* p_column = [[NSTableColumn alloc] init];
            p_column.identifier = [NSString stringWithFormat: @"E %lu", column_id];
            [[p_column headerCell] setStringValue: title];
            [[p_column headerCell] setAlignment: NSCenterTextAlignment];
            [self.detailTableView addTableColumn: p_column];
        }*/
    }
    NSUInteger n_column = [p_table_columns count];
    for( ; column_id < n_column ; column_id++ )
    {
        NSTableColumn* p_column = [p_table_columns objectAtIndex: column_id];
        [[p_column headerCell] setStringValue: @""];
    }
    
    [self.detailTableView setNeedsDisplay: TRUE];
    return TRUE;
}

// -- [ Start action handlers ]
//
- ( NSView* )tableView: ( NSTableView* ) tableView viewForTableColumn:(NSTableColumn *) tableColumn row:(NSInteger)row
{
    if ( tableView == self.processTableView )
    {
        return [self processTableViewForColumn: tableColumn
                                         atRow: row];
    }
    else
    {
        return [self detailScanTableViewForColumn: tableColumn
                                            atRow: row];
    }
}

- ( NSView* ) processTableViewForColumn: ( NSTableColumn *) tableColumn atRow:(NSInteger)row
{
    NSTableCellView* p_cell = [ self.processTableView makeViewWithIdentifier: tableColumn.identifier
                                                           owner: self];
    TProcessInfo& process_info = self->m_pSignal->processList.at( ( size_t)row );
    
    if ( [tableColumn.identifier isEqualToString: @"PID"] )
    {
        p_cell.textField.stringValue = [NSString stringWithFormat:@"%d", process_info.pid ];
        return p_cell;
    }
    
    if ( [tableColumn.identifier isEqualToString: @"ProcessName"])
    {
        p_cell.textField.stringValue = [NSString stringWithFormat:@"%s", process_info.process_name.c_str()];
        return p_cell;
    }
    
    if ( [tableColumn.identifier isEqualToString: @"ProcessPath"])
    {
        p_cell.textField.stringValue = [NSString stringWithFormat: @"%s", process_info.process_path.c_str()];
        return p_cell;
    }
    
    if ( [tableColumn.identifier isEqualToString: @"ScanResult"])
    {
        std::string status = process_info.scan_detail->scan_status;
        NSString* text = nil;        
        
        if ( status != "scanning" )
        {
            int n_engine = process_info.scan_detail->n_engine;
            text = [NSString stringWithFormat: @"%s by %d engine(s)", status.c_str(), n_engine];
        }
        else
        {
            text = [NSString stringWithFormat: @"%s", status.c_str()];
        }
        
        p_cell.textField.stringValue = text;
        
        return p_cell;
    }
    return p_cell;
}

- ( NSView* ) detailScanTableViewForColumn: ( NSTableColumn *) tableColumn atRow:( NSInteger )row
{
    NSTableCellView* p_cell = [ self.detailTableView makeViewWithIdentifier: tableColumn.identifier
                                                                       owner: self];
    
    NSCell* p_header = [tableColumn headerCell];
    if ( [ [p_header stringValue] isEqualToString: @"Information"] == TRUE )
    {
        NSString* text = nil;
        if ( row == 0 )
            text = @"scan result";
        else
            text = @"scan time";
        
        p_cell.textField.stringValue = text;
    }
    else
    {
        NSString* text = @"";
        if ( self->m_selJson.isType( WAAPI_JSON_OBJECT ) )
        {
            std::string s_tile( [[p_header stringValue] UTF8String] );
            WaJson engine_json;
            self->m_selJson.get( WaStringUtils::_T( ( char* )s_tile.c_str() ), engine_json );            
            
            if ( row == 0 )
            {
                wa_int i_scan_result;
                engine_json.get( L"scan_result_i", i_scan_result );
                text = [NSString stringWithFormat: @"%d", i_scan_result];
            }
            else
            {
                wa_int i_scan_time;
                engine_json.get( L"scan_time", i_scan_time );
                text = [NSString stringWithFormat: @"%d ms", i_scan_time];
            }
        }
        
        
        p_cell.textField.stringValue = text;
    }
    return p_cell;
}

- ( NSInteger ) numberOfRowsInTableView: ( NSTableView* ) tableView
{
    if ( self.processTableView == tableView )
    {
        return ( NSInteger )self->m_pSignal->processList.size();
    }
    else
    {
        return 2;
    }
}

- ( void ) tableViewSelectionDidChange: ( NSNotification* ) notification
{
    NSInteger i_selection = [self.processTableView selectedRow];
    [self updateDetailTable: i_selection];
}

- (IBAction)clickOption:(id)sender
{
    OptionWindowController* p_option = [[OptionWindowController alloc] initWithWindowNibName:@"OptionWindowController"];
    
    NSWindow* p_window = [p_option window];
    [NSApp runModalForWindow: p_window];
    
    [p_window orderOut: self];
}

//
// -- [ End action handlers ]



// -- [ Start Local functions ]
//


void receiverSchedule( void *info, CFRunLoopRef rl, CFStringRef mode )
{
    printf( "Call scheduler\n" );
    MasterViewController* p_controller = (__bridge  MasterViewController* )info;
    
    [p_controller setupProcessLister];
}

void receiverPerform (void *info)
{
    MasterViewController* p_controller = (__bridge  MasterViewController* )info;
    [p_controller updateView];
}

//
// -- [ End Local functions ]

@end
