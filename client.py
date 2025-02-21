import socket
import tkinter as tk

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
player = 'x'

class GameApp:
    def __init__(self, window):
        self.player_pos = { 'x': 0, 'y': 2 }
        self.window = window
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

        if GAME_MAP[new_y][new_x] != 'w': # w is not walkable (water)
          self.player_pos['x'] = new_x
          self.player_pos['y'] = new_y
          if (self.player_id): self.canvas.delete(self.player_id)
          self.draw_player()

    def draw_player(self):
      x0 = self.player_pos['x'] * CELL_SIZE
      y0 = self.player_pos['y'] * CELL_SIZE

      self.player_id = self.canvas.create_text(
        (x0 + CELL_SIZE/2, y0 + CELL_SIZE/2),
        text=player,
        fill=COLORS.get(player, "red"),
        font=('Arial', 60)
      )

    def draw_map(self):
      for y, row in enumerate(GAME_MAP):
        for x, symbol in enumerate(row):
          x0 = x * CELL_SIZE
          y0 = y * CELL_SIZE
          x1 = x0 + CELL_SIZE
          y1 = y0 + CELL_SIZE

          self.canvas.create_rectangle(x0, y0, x1, y1, fill=COLORS.get(symbol, "black"), outline="black")

window = tk.Tk()
window.title("Client")
app = GameApp(window)
window.mainloop()

# client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# client.connect(('localhost', 1234))

# player_id = 123
# client.send(player_id.to_bytes(4, byteorder='little'))

# x = 10
# y = 20
# client.send(x.to_bytes(4, byteorder='little'))
# client.send(y.to_bytes(4, byteorder='little'))
