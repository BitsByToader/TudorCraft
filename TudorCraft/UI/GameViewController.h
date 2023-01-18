//
//  SwiftCppBridge.h
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#ifndef SwiftCppBridge_h
#define SwiftCppBridge_h

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

/// The actual class that displays the output of the C++ renderer.  The class inherits `MtkViewDelegate` to be able
/// to receive information from the GPU device. Also, `GameViewController` acts as a bridge of sorts between the C++
/// engine and the rest of the UI written in Swift and using Apple's frameworks.
@interface GameViewController : PlatformViewController <MTKViewDelegate>

/// Creates the view controller with information for connecting to a MC:JE server.
/// - Parameters:
///   - username: Username of the player
///   - ip: IP address of the server
///   - port: Port of the server
- (id) initWithUsername: (NSString*) username ip: (NSString*) ip port: (int) port;

/// Method which informs the engine the a key was pressed down.
/// - Parameter key: Code of the key that was pressed.
- (void) pressedKey: (UInt16) key;

/// Method which informs the engine that a key was released.
/// - Parameter key: Code of the key that was released.
- (void) releasedKey: (UInt16) key;

/// Method which informs the engine that the pointing device(trackpad, touchscreen, etc.) has moved.
/// - Parameters:
///   - x: Relative X coordinate of the movement.
///   - y: Relative X coordinate of the movement.
- (void) mouseMovedWithDeltaX: (CGFloat) x deltaY: (CGFloat) y;

/// Method which informs the engine that a mouse button was pressed. 0 for left button, 1 for right button.
/// - Parameter button: Code of mouse button that was pressed.
- (void) pressedMouse: (int) button;

@end

#endif /* SwiftCppBridge_h */
