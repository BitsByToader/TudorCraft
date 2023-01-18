//
//  ContentView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

import SwiftUI

/// While the `TudorCraftApp` struct is the entry point and is usually responsible for setting up other features
/// like Continuity, DragAndDrop, top bar buttons etc, the `ContentView` struct is what holds the first few bits of the UI.
///
/// Here, it a NavigationStack is the the root of the UI and provides navigation between all the views of the game, the first one
/// being the main screen of the game where the user can select between singleplayer, multiplayer, option or exiting the game.
struct ContentView: View {
    var body: some View {
        NavigationStack {
            MainScreenView()
        }
    }
}
