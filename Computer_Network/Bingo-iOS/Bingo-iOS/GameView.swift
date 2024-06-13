//
//  GameView.swift
//  Bingo-iOS
//
//  Created by Brian Chang on 2024/3/31.
//

import SwiftUI

struct GameView: View {

    @EnvironmentObject private var client: BingoClient

    var body: some View {
        VStack {
            Image("Bingo")
                .resizable()
                .scaledToFit()
                .padding(.horizontal, 70)

            ScrollView(.horizontal) {
                HStack {
                    ForEach(client.players.values) { player in
                        VStack {
                            Text(player.name)
                            if (client.gameStatus != 2) {
                                Text(player.ready ? "Ready" : "Waiting")
                            } else {
                                Text("Score: \(player.score)")
                            }
                        }
                        .padding(5)
                        .overlay(
                            RoundedRectangle(cornerRadius: 8)
                                .foregroundColor(.gray.opacity(0.2))
                        )
                    }
                }
            }
            .padding(.horizontal, 20)

            Spacer()

            Grid {
                ForEach(0..<5) { row in
                    GridRow {
                        ForEach(0..<5) { column in
                            BingoGridItem(row: row, column: column)
                        }
                    }
                }
            }

            Button {
                if client.gameStatus == 0 {
                    client.setReady()
                }
            } label: {
                HStack {
                    Spacer()
                    switch client.gameStatus {
                    case -1:
                        Text("Please choose your numbers")
                    case 0:
                        Text("Ready")
                    case 1:
                        Text("Waiting for other players...")
                    case 2:
                        Text("Score: \(self.client.score)").bold().font(.headline)
                    default:
                        Text("Error")
                    }
                    Spacer()
                }
                .padding(.vertical, 15)
                .foregroundColor(Color.white)
                .background(client.gameStatus == 0 ? .blue : client.gameStatus == 2 ? .yellow : .gray)
                .cornerRadius(12)
            }
        }
        .padding(.horizontal, 10)
        .toolbar { toolbar }
    }

    @ToolbarContentBuilder
        var toolbar: some ToolbarContent {
            ToolbarItem(placement: .navigationBarLeading) {
                Button("Leave") {
                    client.disconnect()
                }
            }

            ToolbarItem(placement: .principal) {
                Text(client.name)
            }
        }
}

struct BingoGridItem: View {
    @EnvironmentObject private var client: BingoClient

    let row: Int, column: Int

    var body: some View {
        Button {
            if client.gridNumbers[row][column].num == 0 {
                client.setGirdNumber(row: row, column: column)
            }
        } label: {
            Rectangle()
                .aspectRatio(0.8, contentMode: .fit)
                .foregroundColor(.clear)
                .overlay {
                    Image(systemName: client.gridNumbers[row][column].num == 0 ? "circle" : "\(client.gridNumbers[row][column].num).circle")
                        .resizable()
                        .scaledToFill()
                        .padding(1)
                        .symbolVariant(client.calledNumbers.contains(client.gridNumbers[row][column].num) ? .fill : .none)
                }
        }
        .buttonStyle(.bordered)
        .tint(client.gridNumbers[row][column].line ? .yellow : client.calledNumbers.contains(client.gridNumbers[row][column].num) ? .green : .blue)
    }
}

#Preview {
    GameView()
}
