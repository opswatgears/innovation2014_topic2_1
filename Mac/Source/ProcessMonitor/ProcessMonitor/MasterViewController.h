//
//  MasterViewController.h
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/25/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "wa_process_def.h"
#include "wa_process_lister.h"

@interface MasterViewController : NSViewController
{
@private
    TSignal*            m_pSignal;
    WaProcessLister*    m_pLister;
    
    WaJson              m_selJson;
}

- ( void ) initialize;
- ( void ) destroy;

- ( void ) setSource: ( CFRunLoopSourceRef ) cfSource andRunner: ( CFRunLoopRef ) runner;
- ( void ) setupProcessReceiver;
- ( void ) setupProcessLister;

- ( void ) updateView;
- ( int ) countScanned;
- ( NSView* ) processTableViewForColumn: ( NSTableColumn *) tableColumn atRow:(NSInteger)row;
- ( NSView* ) detailScanTableViewForColumn: ( NSTableColumn *) tableColumn atRow:(NSInteger)row;
- ( void ) updateDetailTable: ( NSInteger ) selectionId;
- ( BOOL ) fillDetailTitle: ( WaJson* ) p_detail;
- ( NSString* ) filterResult: ( int ) i_scan_result;

void receiverSchedule( void *info, CFRunLoopRef rl, CFStringRef mode );
void receiverPerform (void *info);

@end
