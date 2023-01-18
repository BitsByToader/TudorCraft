//
//  TouchControls.swift
//  TudorCraft
//
//  Created by Tudor Ifrim on 12.01.2023.
//

import SwiftUI

/// `TouchControlsView` is nothing more than what the name suggests. It provides a form of controls for touch devices that don't
/// have a keyboard or mouse (or even if they have, they can't use NSEvent because they're running iOS and not macOS).
struct TouchControlsView: View {
    let controller: GameViewController?
    
    var body: some View {
        HStack {
            VStack {
                Spacer()
                
                HStack {
                    Button(action: {
                        controller?.releasedKey(0)
                    }, label: {
                        Image(systemName: "arrow.left")
                            .foregroundColor(.white)
                            .frame(width: 100, height: 100)
                            .background(.regularMaterial,
                                        in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                    }).onTouchDownGesture {
                        controller?.pressedKey(0)
                    }
                    
                    VStack {
                        Button(action: {
                            controller?.releasedKey(13)
                        }, label: {
                            Image(systemName: "arrow.up")
                                .foregroundColor(.white)
                                .frame(width: 100, height: 100)
                                .background(.regularMaterial,
                                            in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                        }).onTouchDownGesture {
                            controller?.pressedKey(13)
                        }
                        
                        Button(action: {
                            controller?.releasedKey(49)
                        }, label: {
                            Image(systemName: "space")
                                .foregroundColor(.white)
                                .frame(width: 100, height: 100)
                                .background(.regularMaterial,
                                            in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                        }).onTouchDownGesture {
                            controller?.pressedKey(49)
                        }
                        
                        Button(action: {
                            controller?.releasedKey(1)
                        }, label: {
                            Image(systemName: "arrow.down")
                                .foregroundColor(.white)
                                .frame(width: 100, height: 100)
                                .background(.regularMaterial,
                                            in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                        }).onTouchDownGesture {
                            controller?.pressedKey(1)
                        }
                    }
                    
                    Button(action: {
                        controller?.releasedKey(2)
                    }, label: {
                        Image(systemName: "arrow.right")
                            .foregroundColor(.white)
                            .frame(width: 100, height: 100)
                            .background(.regularMaterial,
                                        in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                    }).onTouchDownGesture {
                        controller?.pressedKey(2)
                    }
                }
                
                Spacer()
            }
            
            Spacer()
            
            HStack {
                Button(action: {
                    controller?.pressedMouse(0)
                }, label: {
                    Text("Remove")
                        .bold()
                        .foregroundColor(.white)
                }).frame(width:100, height: 25)
                .background(.regularMaterial,
                            in: RoundedRectangle(cornerRadius: 10, style: .continuous))
                
                Button(action: {
                    controller?.pressedMouse(1)
                }, label: {
                    Text("Place")
                        .bold()
                        .foregroundColor(.white)
                }).frame(width:100, height: 25)
                .background(.regularMaterial,
                            in: RoundedRectangle(cornerRadius: 10, style: .continuous))
            }
        }
    }
}
