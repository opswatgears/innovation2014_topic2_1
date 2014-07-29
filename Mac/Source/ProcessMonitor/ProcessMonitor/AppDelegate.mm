//
//  AppDelegate.m
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/25/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    self.masterViewController = [[MasterViewController alloc] initWithNibName: @"MasterViewController"
                                                                       bundle: nil];
    [self.window.contentView addSubview: self.masterViewController.view];
    self.masterViewController.view.frame = ( ( NSView* )self.window.contentView ).bounds;
}

-( void ) applicationWillTerminate:(NSNotification *)notification
{
    [self.masterViewController destroy];
}

- ( BOOL ) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return TRUE;
}
@end
