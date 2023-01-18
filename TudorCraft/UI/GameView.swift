//
//  GameView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 12.01.2023.
//

import SwiftUI

// TODO: Use the modifiers flags to tell when the Shift/Alt/Ctrl keys are used
// TODO: controller.keyPress should return a boolean on whether or not it used the sent keys

struct GameView: View {
    let username: String
    let address: String
    let port: Int
    
    @State var gamePaused: Bool = false {
        willSet {
            // Press the Esc key
            controller?.releasedKey(53)
        }
    }
    
    @State var controller: GameViewController?
    @State var previousLocation: CGPoint = CGPoint(x: 0, y: 0)
    
    @GestureState private var isTapped = false
    
    var drag: some Gesture {
        DragGesture(minimumDistance: 10, coordinateSpace: .local)
            .onChanged { value in
                controller?.mouseMoved(withDeltaX: (value.location.x - previousLocation.x).rounded(.towardZero),
                                      deltaY: (value.location.y - previousLocation.y).rounded(.towardZero))
                previousLocation = value.location
            }
            .onEnded { value in
                previousLocation = CGPoint(x: 0, y: 0)
                
            }
    }
    
    #if os(macOS)
    @State var monitors: [Any?] = []
    #endif
    
    
    var body: some View {
        ZStack {
            // MARK: - Actual Game view
            if controller != nil {
                GameViewRepresentable(controller: controller!)
                    .edgesIgnoringSafeArea(.all)
                    
                    .onAppear {
                        // MARK: - macOS controls
    #if os(macOS)
                        var monitor: Any? = nil
                        monitor = NSEvent.addLocalMonitorForEvents(matching: .keyDown, handler: { (event) -> NSEvent? in
                            controller?.pressedKey(event.keyCode)
                            
                            return nil
                        })
                        monitors.append(monitor)
                        
                        monitor = NSEvent.addLocalMonitorForEvents(matching: .keyUp, handler: { (event) -> NSEvent? in
                            
                            if (event.keyCode == 53){
                                withAnimation {
                                    gamePaused.toggle()
                                }
                            }
                            else {
                                controller?.releasedKey(event.keyCode)
                            }
                            
                            return nil
                        })
                        monitors.append(monitor)
                        
                        monitor = NSEvent.addLocalMonitorForEvents(matching: .mouseMoved, handler: { (event) -> NSEvent? in
                            controller?.mouseMoved(withDeltaX: event.deltaX, deltaY: event.deltaY)
                            
                            return nil
                        })
                        monitors.append(monitor)
                        
                        monitor = NSEvent.addLocalMonitorForEvents(matching: .leftMouseUp, handler: { (event) -> NSEvent? in
                            controller?.pressedMouse(0)
                            
                            return nil
                        })
                        monitors.append(monitor)
                        
                        monitor = NSEvent.addLocalMonitorForEvents(matching: .rightMouseUp, handler: { (event) -> NSEvent? in
                            controller?.pressedMouse(1)
                            
                            return nil
                        })
                        monitors.append(monitor)
    #endif
                    }.gesture(drag)
                    .onDisappear {
                        #if  os(macOS)
                        for monitor in monitors {
                            NSEvent.removeMonitor(monitor as Any)
                        }
                        monitors = []
                        #endif
                    }
            }
           
            // MARK: - iOS controls
#if os(iOS)
            TouchControlsView(controller: controller)
#endif
            
            // MARK: - HUD
            GameHudView(gamePaused: $gamePaused)
                .onAppear {
                    controller = GameViewController(username: username, ip: address, port: Int32(port))
                }.toolbar(gamePaused ? .visible : .hidden)
        }
    }
}

// MARK: - Touch Down Gesture extension
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
// MARK: -
