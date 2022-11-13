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
    
    var body: some View {
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
            }
    }
}
