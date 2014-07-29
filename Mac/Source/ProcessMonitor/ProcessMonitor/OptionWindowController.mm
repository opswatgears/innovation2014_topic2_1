//
//  OptionWindowController.m
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/27/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#import "OptionWindowController.h"

@interface OptionWindowController ()
@property (weak) IBOutlet NSTextField *textApiKey;
@property (weak) IBOutlet NSComboBox *comboFrequency;
- (IBAction)clickAccept:(id)sender;
- (IBAction)clickCancel:(id)sender;

@end

@implementation OptionWindowController

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (IBAction)clickAccept:(id)sender
{
    [NSApp stopModal];
}

- (IBAction)clickCancel:(id)sender
{
    [NSApp stopModal];
}
@end
