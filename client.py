import socket
import tkinter as tk
import struct
import threading
import random

GAME_MAP = [
  ['w', 'w', 'w', 'w', 'w'],
  ['w', 'o', 'o', 'o', 'o'],
  ['o', 'o', 'o', 'o', 'o'],
  ['o', 'o', 'o', 'o', 'o']
]

COLORS = {
  'o': 'green',
  'w': 'blue',
  'x': 'red'
}

CELL_SIZE = 100
PLAYER_SYMBOL = 'x'

class GameApp:
    def __init__(self, window):
        self.window = window
        self.player_pos = {'x': 0, 'y': 2}
        self.other_players = {} 
        
        # Network setup
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect(('localhost', 1234))
        
        # Send player ID (4 bytes big-endian)
        self.player_id = random.randint(1, 999)  # This should be unique per client
        self.client.send(self.player_id.to_bytes(4, byteorder='big'))
        print(self.player_id)
        # Start receive thread
        self.running = True
        self.receive_thread = threading.Thread(target=self.receive_updates)
        self.receive_thread.start()

        # GUI setup
        self.canvas = tk.Canvas(
            window,
            width=len(GAME_MAP[0]) * CELL_SIZE,
            height=len(GAME_MAP) * CELL_SIZE
        )
        self.canvas.pack()
        self.draw_map()
        self.draw_player()

        self.window.bind("<KeyPress>", self.on_key_press)
        self.canvas.focus_set()
        
        # Handle window close
        self.window.protocol("WM_DELETE_WINDOW", self.on_close)

    def on_close(self):
        self.running = False
        self.client.close()
        self.window.destroy()

    def on_key_press(self, event):
        """Handle arrow key presses"""
        old_x = self.player_pos['x']
        old_y = self.player_pos['y']
        new_x, new_y = old_x, old_y

        if event.keysym == 'Up':
            new_y = max(old_y - 1, 0)
        elif event.keysym == 'Down':
            new_y = min(old_y + 1, len(GAME_MAP) - 1)
        elif event.keysym == 'Left':
            new_x = max(old_x - 1, 0)
        elif event.keysym == 'Right':
            new_x = min(old_x + 1, len(GAME_MAP[0]) - 1)
        else:
            return

        if GAME_MAP[new_y][new_x] != 'w':  # Check if walkable
            self.player_pos['x'] = new_x
            self.player_pos['y'] = new_y
            self.update_player_gui()
            self.send_position()  # Send updated position to server

    def draw_map(self):
        """Draw the game map"""
        for y, row in enumerate(GAME_MAP):
            for x, symbol in enumerate(row):
                x0 = x * CELL_SIZE
                y0 = y * CELL_SIZE
                x1 = x0 + CELL_SIZE
                y1 = y0 + CELL_SIZE
                self.canvas.create_rectangle(
                    x0, y0, x1, y1,
                    fill=COLORS.get(symbol, "black"),
                    outline="black"
                )

    def draw_player(self):
        """Draw the local player"""
        x0 = self.player_pos['x'] * CELL_SIZE
        y0 = self.player_pos['y'] * CELL_SIZE
        self.player_gui = self.canvas.create_text(
            (x0 + CELL_SIZE/2, y0 + CELL_SIZE/2),
            text=PLAYER_SYMBOL,
            fill=COLORS.get(PLAYER_SYMBOL, "red"),
            font=('Arial', 60)
        )

    def update_player_gui(self):
        """Update player position on the GUI"""
        x0 = self.player_pos['x'] * CELL_SIZE
        y0 = self.player_pos['y'] * CELL_SIZE
        self.canvas.coords(self.player_gui, 
                          x0 + CELL_SIZE/2, 
                          y0 + CELL_SIZE/2)

    def send_position(self):
        """Send only coordinates (8 bytes)"""
        data = struct.pack('>ii', self.player_pos['x'], self.player_pos['y'])
        self.client.send(data)

    def receive_updates(self):
        """Receive updates from server in background thread"""
        while self.running:
            try:
                # Read header (1 byte)
                header = self.client.recv(1)
                if not header:
                    break
                    
                # Handle movement packet
                if header == b'\x21':
                    data = self.client.recv(12)
                    if len(data) == 12:
                        player_id = struct.unpack('>I', data[0:4])[0]
                        x = struct.unpack('>i', data[4:8])[0]
                        y = struct.unpack('>i', data[8:12])[0]
                        
                        # Update GUI in main thread
                        self.window.after(0, self.update_other_player, 
                                        player_id, x, y)
            
            except (ConnectionResetError, BrokenPipeError):
                break

    def update_other_player(self, player_id, x, y):
      print(player_id)
      """Update other player's position on GUI"""
      if player_id == self.player_id:
          return  # Ignore self
      
      # Create or update player marker
      if player_id not in self.other_players:
        x0 = x * CELL_SIZE
        y0 = y * CELL_SIZE
        self.other_players[player_id] = self.canvas.create_text(
            (x0 + CELL_SIZE/2, y0 + CELL_SIZE/2),
            text='O',
            fill='orange',
            font=('Arial', 60)
        )
      else:
        x0 = x * CELL_SIZE
        y0 = y * CELL_SIZE
        self.canvas.coords(self.other_players[player_id], x0 + CELL_SIZE/2, y0 + CELL_SIZE/2)

window = tk.Tk()
window.title("Client")
app = GameApp(window)
window.mainloop()
