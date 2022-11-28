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
    
    var drag: some Gesture {
        DragGesture(minimumDistance: 10, coordinateSpace: .local)
            .onChanged { value in
                controller.mouseMoved(value.location.x - previousLocation.x,
                                      newY: value.location.y - previousLocation.y)
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
                        controller.keyPress(event.keyCode)
                        
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
                            controller.keyPress(0)
                        }, label: {
                            Image(systemName: "arrow.left")
                                .frame(width: 100, height: 100)
                        })
                        
                        VStack {
                            Button(action: {
                                controller.keyPress(13)
                            }, label: {
                                Image(systemName: "arrow.up")
                                    .frame(width: 100, height: 100)
                            })
                            
                            Button(action: {
                                controller.keyPress(1)
                            }, label: {
                                Image(systemName: "arrow.down")
                                    .frame(width: 100, height: 100)
                            })
                        }
                        
                        Button(action: {
                            controller.keyPress(2)
                        }, label: {
                            Image(systemName: "arrow.right")
                                .frame(width: 100, height: 100)
                        })
                    }
                    
                    Spacer()
                }
                
                Spacer()
            }
#endif
        }.edgesIgnoringSafeArea(.all)
    }
}
