//
//  ConnectionView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 14.01.2023.
//

import SwiftUI

/// `ConnectionView` is the view that prompts the user to enter all of the details for the server it wants to connet to. The user can
/// choose any arbitrary official *Minecraft: Java Edition*, version **1.19.2** that has compression and encryption disabled and log in
/// with any username he wishes.
struct ConnectionView: View {
    @AppStorage("username") var chosenUsername: String = "TudorCraft"
    @AppStorage("address") var chosenIp: String = "127.0.0.1"
    @AppStorage("port") var chosenPort: String = "25565"
    @State var gamePresented: Bool = false
    
    var body: some View {
        ZStack {
            #if os(iOS)
            Image("background")
                .resizable()
                .scaledToFill()
                .ignoresSafeArea(.all)
            #endif

            
            HStack {
                Spacer()
                
                VStack {
                    TextField("Username", text: $chosenUsername)
                        .textFieldStyle(.plain)
                        .padding()
                        .backgroundStyle(Color.black)
                        .border(Color.white, width: 2)
                        .frame(maxWidth:400)
                    
                    TextField("IP", text: $chosenIp)
                        .textFieldStyle(.plain)
                        .padding()
                        .backgroundStyle(Color.black)
                        .border(Color.white, width: 2)
                        .frame(maxWidth:400)
                    
                    TextField("Port", text: $chosenPort)
                        .textFieldStyle(.plain)
                        .padding()
                        .backgroundStyle(Color.black)
                        .border(Color.white, width: 2)
                        .frame(maxWidth:400)
                    
                    Button(action: {
                        gamePresented = true
                    }, label: {
                        Text("CONNECT")
                            .font(Font.custom("MinecraftFont", size: 18))
                            .frame(width: 300, height: 50)
                            .background(Color.gray)
                            .foregroundColor(.white)
                            .border(.white)
                            .cornerRadius(2)
                    }).navigationDestination(isPresented: $gamePresented, destination: {
                        GameView(username: chosenUsername, address: chosenIp, port: Int(chosenPort) ?? 0)
                        #if os(macOS)
                            .scaledToFill()
                        #endif
                    }).buttonStyle(.plain)
                }
                
                Spacer()
            }
        }
    }
}
