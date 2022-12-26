//
//  SwiftCppBridge.m
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#import "GameViewController.h"

#include "Renderer.hpp"
#include "World.hpp"
#include "TCPClient.hpp"

@implementation GameViewController
    MTKView* m_view;
    
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
    cc.red = 0.0;
    cc.blue = 0.0;
    cc.green = 0.0;
    cc.alpha = 1.0;
    
    m_view.clearColor = cc;
    m_view.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    m_view.clearDepth = 1.0;
    m_view.colorPixelFormat = MTLPixelFormat::MTLPixelFormatBGRA8Unorm_sRGB;
    
    m_view.device = MTLCreateSystemDefaultDevice();
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

- (void) keyPress: (UInt16) key {
    switch (key) {
        case 13:
//            NSLog(@"W");
            m_renderer->forward();
            break;
            
        case 0:
//            NSLog(@"A");
            m_renderer->left();
            break;
            
        case 1:
//            NSLog(@"S");
            m_renderer->backward();
            break;
            
        case 2:
//            NSLog(@"D");
            m_renderer->right();
            break;
            
        case 48:
//            NSLog(@"Tab");
            m_renderer->down();
            break;
            
        case 49:
//            NSLog(@"Space");
            m_renderer->up();
            break;
            
        case 53:
//            NSLog(@"Esc");
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
            break;
            
        case 123:
//            NSLog(@"LeftArrow");
            m_renderer->lookLeft();
            break;
            
        case 124:
//            NSLog(@"RightArrow");
            m_renderer->lookRight();
            break;
            
        case 125:
//            NSLog(@"DownArrow");
            m_renderer->lookDown();
            break;
            
        case 126:
//            NSLog(@"UpArrow");
            m_renderer->lookUp();
            break;
            
        default:
            break;
    }
};

- (void) mouseMoved: (CGFloat) x newY: (CGFloat) y {
    if ( !gamePaused ) {
        if ( x > 0 ) {
            m_renderer->lookRight();
        } else if ( x < 0 ){
            m_renderer->lookLeft();
        }
        
        if ( y < 0 ) {
            m_renderer->lookUp();
        } else if ( y > 0 ) {
            m_renderer->lookDown();
        }
    }
};

- (BOOL)commitEditingAndReturnError:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    return false;
}

- (void)encodeWithCoder:(nonnull NSCoder *)coder {
    //stub
}

@end


