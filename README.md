# LSP Paint

LSP Paint is an app for creating sprites for games made with Lite Engine. It is used to create and modify sprite files stored in the LSP file format (Lite Engine Sprite).

LSP sprites are used for entities and the player's weapons in Lite Engine.

To use created sprites in a game, they must first be included in an LED file using [Lade](https://github.com/pinwinsoftware/Lade).

# Creating Sprites

LSP sprites have a 1:1 aspect ratio. Sprite sizes range from 1x1 to 128x128 pixels.

## Drawing Sprite

### Pixel Types

Transparent — An empty area of the sprite that draws nothing and allows objects behind the entity to be visible.

Void — An empty area that does not draw anything, but prevents objects behind the entity from being visible through this area.

Solid — A solid pixel that is drawn as part of the sprite.

### Sprite Size

You can change the sprite size by clicking the left and right arrows on the sprite size field in the bottom-right corner.
