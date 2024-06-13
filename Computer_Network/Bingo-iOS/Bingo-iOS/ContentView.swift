//
//  ContentView.swift
//  Bingo-iOS
//
//  Created by Brian Chang on 2024/4/7.
//

import SwiftUI

struct ContentView: View {

    @EnvironmentObject private var client: BingoClient
    @State private var nickname = "Brian"
    @State private var server = "http://localhost:3000"
    @FocusState private var focusedField: Field?

    var body: some View {
        if client.isConnected {
            NavigationStack {
                GameView()
            }
            .environmentObject(client)
        } else {
            VStack {
                Spacer()

                Image("Bingo")
                    .resizable()
                    .scaledToFit()
                    .padding(40)

                Spacer()

                TextField("Nickname", text: $nickname)
                    .focused($focusedField, equals: .Nickname)
                    .padding(10)
                    .overlay(
                        RoundedRectangle(cornerRadius: 5)
                            .stroke(
                                (focusedField == .Nickname ? .yellow : Color(red: 209/255, green: 209/255, blue: 213/255)),
                                lineWidth: (focusedField == .Nickname ? 2 : 1)
                            )
                    )

                TextField("Server URL", text: $server)
                    .focused($focusedField, equals: .Server)
                    .padding(10)
                    .overlay(
                        RoundedRectangle(cornerRadius: 5)
                            .stroke(
                                (focusedField == .Server ? .yellow : Color(red: 209/255, green: 209/255, blue: 213/255)),
                                lineWidth: (focusedField == .Server ? 2 : 1)
                            )
                    )

                Button {
                    client.connect(serverURL: server, with: nickname)
                } label: {
                    HStack {
                        Spacer()
                        Text("Connect")
                            .padding(.vertical, 10)
                            .foregroundColor(Color.white)
                        Spacer()
                    }.background(.blue).cornerRadius(12)
                }
                .padding(.bottom)

                Spacer()
            }
            .padding()
        }
    }

    enum Field: Hashable {
        case Nickname, Server
    }
}
