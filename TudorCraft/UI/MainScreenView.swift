//
//  MainScreenView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 13.01.2023.
//

import SwiftUI

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
