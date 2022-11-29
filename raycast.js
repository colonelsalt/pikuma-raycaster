const TILE_SIZE = 64;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

const FOV_ANGLE = 60 * (Math.PI / 180);

const WALL_STRIP_WIDTH = 1;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH;

class Map {
    constructor() {
        this.grid = [
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        ];
    }

    wallAt(x, y) {
        if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
            return true;
        }
        const gridX = Math.floor(x / TILE_SIZE);
        const gridY = Math.floor(y / TILE_SIZE);
        return this.grid[gridY][gridX] == 1
    }

    render() {
        for (var i = 0; i < MAP_NUM_ROWS; i++) {
            for (var j = 0; j < MAP_NUM_COLS; j++) {
                var tileX = j * TILE_SIZE; 
                var tileY = i * TILE_SIZE;
                var tileColor = this.grid[i][j] == 1 ? "#222" : "#fff";
                stroke("#222");
                fill(tileColor);
                rect(tileX, tileY, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

class Player {
    constructor() {
        this.x = WINDOW_WIDTH / 2.0;
        this.y = WINDOW_HEIGHT / 2.0;
        this.radius = 3;
        this.turnDirection = 0;
        this.walkDirection = 0;
        this.rotationAngle = Math.PI / 2.0;
        this.moveSpeed = 2.0;
        this.rotationSpeed = 2 * (Math.PI / 180);
    }

    update() {
        const moveStep = this.walkDirection * this.moveSpeed;
        
        this.rotationAngle += this.turnDirection * this.rotationSpeed;
        const newX = this.x + moveStep * Math.cos(this.rotationAngle);
        const newY = this.y + moveStep * Math.sin(this.rotationAngle);

        if (!grid.wallAt(newX, newY)) {
            this.x = newX;
            this.y = newY;
        }

    }

    render() {
        noStroke();
        fill("red");
        circle(this.x, this.y, this.radius);
        stroke("red");
        //line(this.x, this.y,
        //     this.x + 30 * Math.cos(this.rotationAngle), this.y + 30 * Math.sin(this.rotationAngle)
        //);
    }
}

class Ray {
    constructor(rayAngle) {
        this.rayAngle = normalizeAngle(rayAngle);
        this.wallHitX = 0;
        this.wallHitY = 0;
        this.distance = 0;
        this.wasHitVertical = false;

        this.isFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
        this.isFacingRight = this.rayAngle < 0.5 * Math.PI || this.rayAngle > 1.5 * Math.PI;
    }

    cast(columnId) {
        // Horizontal intersections
        let yintercept = Math.floor(player.y / TILE_SIZE) * TILE_SIZE;
        if (this.isFacingDown) {
            yintercept += TILE_SIZE;
        }
        let xintercept = player.x + ((yintercept - player.y) / Math.tan(this.rayAngle));
        let ystep = TILE_SIZE;
        if (!this.isFacingDown) {
            ystep *= -1;
        }

        let xstep = TILE_SIZE / Math.tan(this.rayAngle);
        if ((!this.isFacingRight && xstep > 0) || (this.isFacingRight && xstep < 0)) {
            xstep *= -1;
        }
        let nextIntersectionX = xintercept;
        let nextIntersectionY = yintercept;
        
        let horizontalWallHitFound = false;
        let horizontalWallHitX = 0;
        let horizontalWallHitY = 0;

        while (nextIntersectionX >= 0 && nextIntersectionX <= WINDOW_WIDTH
            && nextIntersectionY >= 0 && nextIntersectionY <= WINDOW_HEIGHT) {
            if (grid.wallAt(nextIntersectionX, nextIntersectionY - (this.isFacingDown ? 0 : 1))) {
                horizontalWallHitFound = true;
                horizontalWallHitX = nextIntersectionX;
                horizontalWallHitY = nextIntersectionY;
                break;
            } else {
                nextIntersectionX += xstep;
                nextIntersectionY += ystep;
            }
        }

        // Vertical intersections
        xintercept = Math.floor(player.x / TILE_SIZE) * TILE_SIZE;
        if (this.isFacingRight) {
            xintercept += TILE_SIZE;
        }
        yintercept = player.y + ((xintercept - player.x) * Math.tan(this.rayAngle));
        xstep = TILE_SIZE;
        if (!this.isFacingRight) {
            xstep *= -1;
        }

        ystep = TILE_SIZE * Math.tan(this.rayAngle);
        if ((!this.isFacingDown && ystep > 0) || (this.isFacingDown && ystep < 0)) {
            ystep *= -1;
        }
        nextIntersectionX = xintercept;
        nextIntersectionY = yintercept;
        
        let verticalWallHitFound = false;
        let verticalWallHitX = 0;
        let verticalWallHitY = 0;

        while (nextIntersectionX >= 0 && nextIntersectionX <= WINDOW_WIDTH
            && nextIntersectionY >= 0 && nextIntersectionY <= WINDOW_HEIGHT) {
            if (grid.wallAt(nextIntersectionX - (this.isFacingRight ? 0 : 1), nextIntersectionY)) {
                verticalWallHitFound = true;
                verticalWallHitX = nextIntersectionX;
                verticalWallHitY = nextIntersectionY;
                break;
            } else {
                nextIntersectionX += xstep;
                nextIntersectionY += ystep;
            }
        }

        const xDist = horizontalWallHitFound ? distance(player.x, player.y, horizontalWallHitX, horizontalWallHitY) : Number.MAX_VALUE;
        const yDist = verticalWallHitFound ? distance(player.x, player.y, verticalWallHitX, verticalWallHitY) : Number.MAX_VALUE;

        if (xDist < yDist) {
            this.wallHitX = horizontalWallHitX;
            this.wallHitY = horizontalWallHitY;
            this.distance = xDist;
        } else {
            this.wallHitX = verticalWallHitX;
            this.wallHitY = verticalWallHitY;
            this.distance = yDist;
            this.wasHitVertical = true;
        }
    }

    render() {
        stroke("rgba(255, 0, 0, 0.3)");
        line(player.x, player.y, this.wallHitX, this.wallHitY);
    }
}


var grid = new Map();
var player = new Player();
var rays = [];

function keyPressed() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = 1;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = -1;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = 1;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = -1;
    }
}

function keyReleased() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = 0;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = 0;
    }
}

function castAllRays() {
    var columnId = 0;

    var rayAngle = player.rotationAngle - (FOV_ANGLE / 2.0);
    rays = [];
    for (var i = 0; i < NUM_RAYS; i++) {
        var ray = new Ray(rayAngle);
        ray.cast(columnId);

        rays.push(ray);
        rayAngle += FOV_ANGLE / NUM_RAYS;
        columnId++;
    }
}

function distance(px, py, qx, qy) {
    xDiff = qx - px;
    yDiff = qy - py;
    return Math.sqrt((xDiff * xDiff) + (yDiff * yDiff));
}

function normalizeAngle(angle) {
    angle %= (2 * Math.PI);
    if (angle < 0) {
        angle += 2 * Math.PI;
    }
    return angle;
}

function setup() {
    createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);

}

function update() {
    // TODO: update all game objects before we render the next frame
    player.update();
    castAllRays();
}

function draw() {
    update();

    grid.render();
    for (ray of rays) {
        ray.render();
    }
    player.render();
}
