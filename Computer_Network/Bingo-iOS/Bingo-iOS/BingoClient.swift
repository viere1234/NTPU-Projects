//
//  BingoClient.swift
//  Bingo-iOS
//
//  Created by Brian Chang on 2024/3/30.
//

import Foundation
import SocketIO
import OSLog
import OrderedCollections

let logger = Logger(subsystem: "com.brianchange.bingo", category: "Bingo_Client")

class BingoClient: ObservableObject {
    private var manager: SocketManager?
    private var socket: SocketIOClient?

    // MARK: - Server Information
    @Published var id = ""
    @Published var name = ""
    @Published var isConnected = false
    @Published var gameStatus = -1
    @Published var players: OrderedDictionary<String, Player> = [:]
    @Published var currentNewNumber = -1
    @Published var calledNumbers: [Int] = []

    // MARK: - User Information
    @Published var score = 0
    @Published var gridNumbers: [[BingoItem]] = .init(repeating: .init(repeating: BingoItem(num: 0), count: 5), count: 5)
    @Published var selectedNumbers: Int = 0

    private var flatGridNumbers: [Int] = []

    func connect(serverURL: String, with name: String) {
        guard let url = URL(string: serverURL) else {
            print("Invalid server URL")
            return
        }

        self.name = name

        manager = SocketManager(socketURL: url, config: [.log(true), .compress])
        socket = manager?.defaultSocket

        socket?.on(clientEvent: .connect) { data, _ in
            logger.info("Server Connected")
            // Initial Game
            self.gameStatus = -1
            self.gridNumbers = .init(repeating: .init(repeating: BingoItem(num: 0), count: 5), count: 5)
            self.selectedNumbers = 0
            self.players = [:]
            self.score = 0
            self.socket?.emit("setName", self.name)
            self.isConnected = true
        }

        socket?.on(clientEvent: .disconnect) { data, _ in
            logger.info("Server Disconnected")
            self.isConnected = false
        }

        socket?.on("clientId") { data, _ in
            if let clientId = data.first as? String {
                logger.info("Recieve id: \(clientId)")
                self.id = clientId
            }
        }

        socket?.on("updateClients") { data, _ in
            if let datas = data.first as? [String: [String: Any]] {
                self.players = [:]
                for (clientId, clientData) in datas {
                    guard clientId != self.id else { continue }
                    logger.info("Recieve player info: \(clientId)")
                    self.players[clientId] = .init(
                        id: clientId,
                        name: clientData["name"] as? String ?? "",
                        ready: clientData["ready"] as? Bool ?? false,
                        score: clientData["score"] as? Int ?? 0
                    )
                }
            }
        }

        socket?.on("updateReadiness") { data, _ in
            if let updatedClientId = data.first as? String {
                logger.info("Recieve updated readiness with client: \(updatedClientId)")
                self.players[updatedClientId]?.ready = true
            }
        }

        socket?.on("updateCalledNumbers") { data, _ in
            if let numbers = data.first as? [Int] {
                logger.info("Recieve update numbers")
                self.calledNumbers = numbers
                self.checkNewLine()
            }
        }

        socket?.on("newNumber") { data, _ in
            if let number = data.first as? Int {
                logger.info("Recieve new called number")
                self.currentNewNumber = number
            }
        }

        socket?.on("startGame") { _, _ in
            self.flatGridNumbers = self.gridNumbers.flatMap { $0.compactMap { $0.num } }
            self.gameStatus = 2
        }

        socket?.on("restartGame") { _, _ in
            self.gameStatus = -1
            self.calledNumbers = []
            self.currentNewNumber = -1
            self.gridNumbers = .init(repeating: .init(repeating: BingoItem(num: 0), count: 5), count: 5)
            self.selectedNumbers = 0
        }

        socket?.connect()
    }

    func setGirdNumber(row: Int, column: Int) {
        self.selectedNumbers += 1
        self.gridNumbers[row][column].num = self.selectedNumbers
        if (selectedNumbers == 25) {
            self.gameStatus = 0
        }
    }

    func disconnect() {
        socket?.disconnect()
    }

    func setReady() {
        self.gameStatus = 1
        socket?.emit("setReady")
    }

    func checkNewLine() {
        guard let index = self.flatGridNumbers.firstIndex(of: self.currentNewNumber) else { return }
        let row = index / 5
        let column = index % 5
        var rowMatch = true, columnMatch = true
        for i in 0 ..< 5 {
            if !self.calledNumbers.contains(self.gridNumbers[row][i].num) {
                print("no row \(i)")
                rowMatch = false
                break
            }
        }
        for i in 0 ..< 5 {
            if !self.calledNumbers.contains(self.gridNumbers[i][column].num) {
                print("no column: \(i)")
                columnMatch = false
                break
            }
        }
        if rowMatch || columnMatch {
            if rowMatch { for i in 0 ..< 5 { self.gridNumbers[row][i].line = true } }
            if columnMatch { for i in 0 ..< 5 { self.gridNumbers[i][column].line = true } }
            self.score += rowMatch && columnMatch ? 2 : 1
            self.socket?.emit("sendScore", self.score)

        }
    }
}

struct Player: Identifiable {
    internal let id: String
    var name: String
    var ready: Bool
    var score: Int
}

struct BingoItem {
    var num: Int
    var line = false
}

