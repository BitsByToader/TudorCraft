//
//  SwiftCppBridge.m
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#import "GameViewController.h"

#include "Renderer.h"

@implementation GameViewController
    MTKView* m_view;
    Renderer* m_renderer;
    MTL::Device* m_device;

- (id) init {
    NSLog(@"Called GameViewController init");
    
    self = [super init];
    
    return self;
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
    cc.red = 0.0;
    cc.blue = 0.0;
    cc.green = 0.0;
    cc.alpha = 1.0;
    
    m_view.clearColor = cc;
    
    m_device = MTL::CreateSystemDefaultDevice();
    
    NSAssert(m_device, @"Metal is not supported on this device");
    
    // Configure the view to use the default device
    m_view.device = (__bridge id<MTLDevice>)m_device;
    m_view.delegate = self;
    
    m_renderer = new Renderer(m_device);
    m_view.colorPixelFormat = MTLPixelFormat::MTLPixelFormatBGRA8Unorm_sRGB;
}

- (void)drawInMTKView:(nonnull MTKView *)view {
//    NSLog(@"called drawInMTKView");
    
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
//    if(m_view.paused) {
//        [m_view draw];
//    }
}

- (BOOL)commitEditingAndReturnError:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    return false;
}

- (void)encodeWithCoder:(nonnull NSCoder *)coder {
    //stub
}

@end


