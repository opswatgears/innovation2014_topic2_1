//
//  AppDelegate.h
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/25/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MasterViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (nonatomic, strong) IBOutlet MasterViewController* masterViewController;

@end
