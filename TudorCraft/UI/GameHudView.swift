//
//  GameHudView.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 15.01.2023.
//

import SwiftUI

struct GameHudView: View {
    @Binding var gamePaused: Bool
    
    @State var slots: [Slot] = [
        Slot(name: "grass", selected: true),
        Slot(name: "dirt", selected: false),
        Slot(name: "stone", selected: false),
        Slot(name: "sand", selected: false),
        Slot(name: "plank", selected: false),
        Slot(name: "log", selected: false),
        Slot(name: "leaves", selected: false),
        Slot(name: "tnt", selected: false),
        Slot(name: "diamond_ore", selected: false)
    ]
    
    var body: some View {
        HStack {
            Spacer()
            
            VStack {
                Button(action: {
                    withAnimation {
                        gamePaused.toggle()
                    }
                }, label: {
                    Image(systemName: "escape")
                        .frame(width: 50, height: 50)
                        .foregroundColor(.white)
                        .background(.regularMaterial,
                                    in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                }).buttonStyle(.plain)
                
                Spacer()
                
                Image(systemName: "plus")
                    .resizable()
                    .frame(width: 20, height: 20)
                
                Spacer()
                
                HStack {
                    ForEach(slots.indices) { index in
                        Image(slots[index].name)
                            .resizable()
                            .frame(width: slots[index].selected ? 60 : 40,
                                   height: slots[index].selected ? 60 : 40)
                            .background(.regularMaterial,
                                        in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                            .onTapGesture {
                                NotificationCenter.default
                                    .post(name: Notification.Name("slot"), object: index)
                            }
                    }
                }
            }
            
            Spacer()
        }.onReceive(NotificationCenter.default.publisher(for: Notification.Name("slot")),
                    perform: { notification in
            let slot: Int = notification.object as? Int ?? 0
            
            withAnimation {
                for idx in slots.indices {
                    slots[idx].selected = false
                }
                
                slots[slot].selected = true
            }
        })
    }
}

struct Slot: Identifiable {
    var id = UUID()
    var name: String
    var selected: Bool
}
