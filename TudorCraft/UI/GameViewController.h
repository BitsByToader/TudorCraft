//
//  SwiftCppBridge.h
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#ifndef SwiftCppBridge_h
#define SwiftCppBridge_h

/*#import <Foundation/Foundation.h>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

@interface SwiftCppBridge: NSObject

- (void) initAdapterWithview: (void *) view;
- (void) initRendererWithDefaultDevice;
- (void) testRenderer;
- (void) drawUsingRenderer;


@end*/

#include <Availability.h>

#ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
#import  <UIKit/UIKit.h>
#define PlatformViewController UIViewController
#else
#import <AppKit/AppKit.h>
#define PlatformViewController NSViewController
#endif

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

// The view controller
@interface GameViewController : PlatformViewController <MTKViewDelegate>

- (id) initWithUsername: (NSString*) username ip: (NSString*) ip port: (int) port;
- (void) pressedKey: (UInt16) key;
- (void) releasedKey: (UInt16) key;
- (void) mouseMovedWithDeltaX: (CGFloat) x deltaY: (CGFloat) y;
- (void) pressedMouse: (int) button;

@end

#endif /* SwiftCppBridge_h */
