//
//  SwiftCppBridge.m
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#import "GameViewController.h"

#include "Engine.hpp"
#include "Renderer.hpp"
#include "World.hpp"
#include "TCPClient.hpp"

@implementation GameViewController
    MTKView* m_view;
    
    NSString* m_username;
    NSString* m_address;
    int m_port;

    Engine* m_engine;
    Renderer* m_renderer;
    World* m_world;
    TCPClient* m_client;

    bool gamePaused = false;

- (id) initWithUsername: (NSString*) username ip: (NSString*) ip port: (int) port {
    NSLog(@"Called initWithUsername");
    
    self = [super init];
    
    m_username = username;
    m_address = ip;
    m_port = port;
    
//    [[NSNotificationCenter defaultCenter] addObserver:self
//            selector:@selector(receiveSlotNotification:)
//            name:@"slot"
//            object:nil];
    
    return self;
};

- (void) dealloc {
    NSLog(@"Called GameViewController dealloc");
    
//    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void) viewDidDisappear {
    NSLog(@"Called viewDidDissapear");
    
    delete m_client;
    Renderer::destroySharedObject();
    World::destroySharedObject();
    Engine::destroySharedObject();
    
    m_engine = nullptr;
    m_renderer = nullptr;
    m_world = nullptr;
    m_client = nullptr;
}

- (void) viewDidAppear {
    NSLog(@"Called viewDidAppear");
}

- (void) loadView {
    NSLog(@"Called loadView");
    
    m_view = [[MTKView alloc] init];
    self.view = m_view;
}

- (void)viewDidLoad {
    NSLog(@"Called viewDidLoad");
    
    [super viewDidLoad];
    
    MTLClearColor cc;
    cc.red = 0.5;
    cc.green = 0.83;
    cc.blue = 1.0;
    cc.alpha = 1.0;
    
    m_view.clearColor = cc;
    m_view.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    m_view.clearDepth = 1.0;
    m_view.colorPixelFormat = MTLPixelFormat::MTLPixelFormatBGRA8Unorm_sRGB;
    
    m_view.device = MTLCreateSystemDefaultDevice();
    
    if ( m_username && m_address ) {
        m_renderer = Renderer::shared();
        m_world = World::shared();
        m_engine = Engine::shared();
        m_client = new TCPClient([m_username UTF8String], [m_address UTF8String], m_port);
    }
    
    m_view.delegate = self;
    
#if TARGET_OS_IPHONE
#else
    CGAssociateMouseAndMouseCursorPosition(gamePaused);
    if ( !gamePaused ) {
        CGDisplayHideCursor(kCGDirectMainDisplay);
    }
#endif
}

- (void) receiveSlotNotification: (NSNotification *) notification {
    if ( [notification.name  isEqual: @"slot"] ) {
        
    }
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    // Update the game.
    if ( m_engine != nullptr )
        m_engine->updateGame((__bridge MTL::RenderPassDescriptor *) m_view.currentRenderPassDescriptor,
                             (__bridge MTL::Drawable *) m_view.currentDrawable);
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    if (m_renderer != nullptr )
        m_renderer->windowSizeWillChange(size.width, size.height);
    
    // The drawable changes its size after the view redraws itself.
    // If the user pauses the view, the renderer
    // must explicitly force the view to redraw itself, otherwise Core Animation will just stretch or squish
    // the drawable as the view changes sizes.
    if(m_view.paused) {
        [m_view draw];
    }
}

- (void) pressedKey: (UInt16) key {
    m_engine->keyDown(key);
};

- (void) releasedKey: (UInt16) key {
    if ( key == 53 ) {
        gamePaused = !gamePaused;
        m_view.paused = gamePaused;
#if TARGET_OS_IPHONE
#else
        CGAssociateMouseAndMouseCursorPosition(gamePaused);
        if ( !gamePaused ) {
            CGDisplayHideCursor(kCGDirectMainDisplay);
        } else {
            CGDisplayShowCursor(kCGDirectMainDisplay);
        }
#endif
    } else {
        m_engine->keyUp(key);
    }
};

- (void) mouseMovedWithDeltaX: (CGFloat) x deltaY: (CGFloat) y {
    if ( !gamePaused ) {
        m_engine->mouseMoved(x, y);
    }
};

- (void) pressedMouse: (int) button {
    m_engine->pressedMouse(button);
};

- (BOOL)commitEditingAndReturnError:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    return false;
}

- (void)encodeWithCoder:(nonnull NSCoder *)coder {
    //stub
}

@end


