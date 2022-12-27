//
//  ContentView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

import SwiftUI

// TODO: Use the modifiers flags to tell when the Shift/Alt/Ctrl keys are used
// TODO: controller.keyPress should return a boolean on whether or not it used the sent keys

struct ContentView: View {
    let controller: GameViewController = GameViewController(nibName: nil, bundle: nil)
    @State var previousLocation: CGPoint = CGPoint(x: 0, y: 0)
    
    @GestureState private var isTapped = false
    
    var drag: some Gesture {
        DragGesture(minimumDistance: 10, coordinateSpace: .local)
            .onChanged { value in
                controller.mouseMoved((value.location.x - previousLocation.x).rounded(.towardZero),
                                      newY: (value.location.y - previousLocation.y).rounded(.towardZero))
                previousLocation = value.location
            }
            .onEnded { value in
                previousLocation = CGPoint(x: 0, y: 0)
                
            }
    }
    
    var body: some View {
        ZStack {
            GameView(controller: controller)
                .onAppear {
#if os(macOS)
                    NSEvent.addLocalMonitorForEvents(matching: .keyDown, handler: { (event) -> NSEvent? in
                        controller.pressedKey(event.keyCode)
                        
                        return nil
                    })
                    
                    NSEvent.addLocalMonitorForEvents(matching: .keyUp, handler: { (event) -> NSEvent? in
                        controller.releasedKey(event.keyCode)
                        
                        return nil
                    })
                    
                    NSEvent.addLocalMonitorForEvents(matching: .mouseMoved, handler: { (event) -> NSEvent? in
                        controller.mouseMoved(event.deltaX, newY: event.deltaY)
                        
                        return nil
                    })
#endif
                }.gesture(drag)
            
#if os(iOS)
            HStack {
                VStack {
                    Spacer()
                    
                    HStack {
                        Button(action: {
                            controller.releasedKey(0)
                        }, label: {
                            Image(systemName: "arrow.left")
                                .frame(width: 100, height: 100)
                        }).onTouchDownGesture {
                            controller.pressedKey(0)
                        }
                        
                        VStack {
                            Button(action: {
                                controller.releasedKey(13)
                            }, label: {
                                Image(systemName: "arrow.up")
                                    .frame(width: 100, height: 100)
                            }).onTouchDownGesture {
                                controller.pressedKey(13)
                            }
                            
                            Button(action: {
                                controller.releasedKey(49)
                            }, label: {
                                Image(systemName: "space")
                                    .frame(width: 100, height: 100)
                            }).onTouchDownGesture {
                                controller.pressedKey(49)
                            }
                            
                            Button(action: {
                                controller.releasedKey(1)
                            }, label: {
                                Image(systemName: "arrow.down")
                                    .frame(width: 100, height: 100)
                            }).onTouchDownGesture {
                                controller.pressedKey(1)
                            }
                        }
                        
                        Button(action: {
                            controller.releasedKey(2)
                        }, label: {
                            Image(systemName: "arrow.right")
                                .frame(width: 100, height: 100)
                        }).onTouchDownGesture {
                            controller.pressedKey(2)
                        }
                    }
                    
                    Spacer()
                }
                
                Spacer()
            }
#endif
        }.edgesIgnoringSafeArea(.all)
    }
}

extension View {
    func onTouchDownGesture(callback: @escaping () -> Void) -> some View {
        modifier(OnTouchDownGestureModifier(callback: callback))
    }
}

private struct OnTouchDownGestureModifier: ViewModifier {
    @State private var tapped = false
    let callback: () -> Void

    func body(content: Content) -> some View {
        content
            .simultaneousGesture(DragGesture(minimumDistance: 0)
                .onChanged { _ in
                    if !self.tapped {
                        self.tapped = true
                        self.callback()
                    }
                }
                .onEnded { _ in
                    self.tapped = false
                })
    }
}
