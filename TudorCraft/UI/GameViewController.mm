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
    
    Engine* m_engine;
    Renderer* m_renderer;
    World* m_world;
    TCPClient* m_client;

    bool gamePaused = false;

- (id) init {
//    NSLog(@"Called GameViewController init");
    
    self = [super init];
    
    return self;
}

- (void) viewDidDisappear {
    delete m_renderer;
    delete m_client;
}

- (void) loadView {
//    NSLog(@"Called loadView");
    
    m_view = [[MTKView alloc] init];
    
    self.view = m_view;
}

- (void)viewDidLoad {
//    NSLog(@"Called viewDidLoad");
    
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
    m_engine = Engine::shared();
    m_renderer = Renderer::shared();
    m_world = World::shared();
    
    // Configure the view to use the default device
    m_view.delegate = self;
    
#if TARGET_OS_IPHONE
#else
    CGAssociateMouseAndMouseCursorPosition(gamePaused);
    if ( !gamePaused ) {
        CGDisplayHideCursor(kCGDirectMainDisplay);
    }
#endif
    
    m_client = new TCPClient("192.168.1.11", 25565); //192.168.1.11
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    // Update the world.
    m_engine->updateGame();
    
    // Draw world to screen.
    m_renderer->draw((__bridge MTL::RenderPassDescriptor *) m_view.currentRenderPassDescriptor,
                     (__bridge MTL::Drawable *) m_view.currentDrawable);
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
//    NSLog(@"Called mtkView");
//    MTL::Size newSize = MTL::Size::Make(size.width, size.height, 0);
//    m_renderer->drawableSizeWillChange(newSize, MTL::StorageModePrivate);
    
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

- (void) mouseMoved: (CGFloat) x newY: (CGFloat) y {
    if ( !gamePaused ) {
        m_engine->mouseMoved(x, y);
    }
};

- (BOOL)commitEditingAndReturnError:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    return false;
}

- (void)encodeWithCoder:(nonnull NSCoder *)coder {
    //stub
}

@end


