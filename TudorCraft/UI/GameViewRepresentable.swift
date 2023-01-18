//
//  GameView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

import Foundation
import SwiftUI
import MetalKit

#if os(macOS)

/// This is a bridge between SwiftUI (what the rest of the UI is made of) and GameViewController, a UIKit/Appkit view controller,
/// that hold a derived MetalKitViewController for rendering the game.
struct GameViewRepresentable: NSViewControllerRepresentable {
    typealias NSViewControllerType = GameViewController
    var controller: GameViewController
    
    func makeNSViewController(context: Context) -> GameViewController {
        return controller
    }
    
    func updateNSViewController(_ nsViewController: GameViewController, context: Context) {
        //stub
    }
}
#endif

#if os(iOS)
/// This is a bridge between SwiftUI (what the rest of the UI is made of) and GameViewController, a UIKit/Appkit view controller,
/// that holds a derived MetalKitViewController for rendering the game.
struct GameViewRepresentable: UIViewControllerRepresentable {
    var controller: GameViewController
    
    func makeUIViewController(context: Context) -> GameViewController {
        return controller
    }
    
    func updateUIViewController(_ nsViewController: GameViewController, context: Context) {
        //stub
    }
}
#endif
