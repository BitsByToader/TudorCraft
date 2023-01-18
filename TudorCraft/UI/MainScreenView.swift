//
//  MainScreenView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 13.01.2023.
//

import SwiftUI

/// `MainScreenView` is the first thing the user actually sees. It features a panorama of a desert rendered in-game, and blurred using a
/// gaussian blur. On top of all of this, there's the TudorCraft logo and the options that the user has for proceeding further.
///
/// Currently, only multiplayer is supported and there are no options to tweak, so only the "Multiplayer" button is available.
struct MainScreenView: View {
    @State var isPresented: Bool = false
    
    var body: some View {
        ZStack {
            Image("panorama")
                .resizable()
                .scaledToFill()
                .blur(radius: 10)
                .ignoresSafeArea(.all)
            
            HStack {
                Spacer()
                
                VStack {
                    Image("logo")
                        .resizable()
                        .frame(width: 500, height: 100)
                        .padding(.top, 100)
                    
                    Spacer()
                    
                    Button(action: {
                        isPresented = true
                        
                    }, label: {
                        Text("MULTIPLAYER")
                            .font(Font.custom("MinecraftFont", size: 18))
                            .frame(width: 300, height: 50)
                            .background(Color.gray)
                            .foregroundColor(.white)
                            .border(.white)
                            .cornerRadius(2)
                    }).navigationDestination(isPresented: $isPresented) {
                        ConnectionView()
                    }.buttonStyle(.plain)
                    
                    Spacer()
                }
                
                Spacer()
            }
        }
    }
}
