//
//  AppDelegate.m
//  Window
//
//  Created by ROHAN WAGHMODE on 10/03/20.
//

#import "AppDelegate.h"

#import "ViewController.h"

#import "GLESView.h"

@implementation AppDelegate
{
@private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    GLESView *glesView;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // get screen bounds for fullscreen
    CGRect screenBounds=[[UIScreen mainScreen]bounds];
    
    // initialize window variable corresponding to screen bounds
    mainWindow=[[UIWindow alloc]initWithFrame:screenBounds];
    
    mainViewController=[[ViewController alloc]init];
    
    [mainWindow setRootViewController:mainViewController];
    
    // initialize view variable corresponding to screen bounds
    glesView = [[GLESView alloc]initWithFrame:screenBounds];
    
    [mainViewController setView:glesView];
    
    [glesView release];
    
    // add the ViewController's view as subview to the window
    [mainWindow addSubview:[mainViewController view]];
    
    // make window key window and visible
    [mainWindow makeKeyAndVisible];
    
    [glesView startAnimation];
    
    return(YES);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // code
    [glesView stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // code
    [glesView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // code
    [glesView stopAnimation];
}

- (void)dealloc
{
    // code
    [glesView release];
    
    [mainViewController release];
    
    [mainWindow release];
    
    [super dealloc];
}

@end
