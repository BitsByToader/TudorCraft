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

struct GameView: NSViewControllerRepresentable {
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
struct GameView: UIViewControllerRepresentable {
    var controller: GameViewController
    
    func makeUIViewController(context: Context) -> GameViewController {
        return controller
    }
    
    func updateUIViewController(_ nsViewController: GameViewController, context: Context) {
        //stub
    }
}
#endif
