//
//  BingoApp.swift
//  Bingo
//
//  Created by Brian Chang on 2024/3/30.
//

import SwiftUI

@main
struct Bingo_iOSApp: App {
    @StateObject private var client = BingoClient()
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(client)
        }
    }
}
