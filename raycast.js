const TILE_SIZE = 64;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

const FOV_ANGLE = 60 * (Math.PI / 180);

const WALL_STRIP_WIDTH = 1;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH;

const MINIMAP_SCALE_FACTOR = 0.2;

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
                scaledRect(tileX, tileY, TILE_SIZE, TILE_SIZE);
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
        scaledCircle(this.x, this.y, this.radius);
        stroke("red");
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
        stroke("rgba(255, 0, 0, 1)");
        scaledLine(player.x, player.y, this.wallHitX, this.wallHitY);
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

function renderProjectedWalls() {
    for (var i = 0; i < NUM_RAYS; i++) {
        const ray = rays[i];
        const rayDistance = ray.distance * Math.cos(ray.rayAngle - player.rotationAngle);

        const projPlaneDist = (WINDOW_WIDTH / 2) / Math.tan(FOV_ANGLE / 2);
        const wallStripHeight = (TILE_SIZE / rayDistance) * projPlaneDist;

        const alpha = 1 - (rayDistance / WINDOW_WIDTH);
        fill(`rgba(255, 255, 255, ${alpha})`);
        noStroke();
        rect(
            i * WALL_STRIP_WIDTH,
            (WINDOW_HEIGHT / 2) - (wallStripHeight / 2),
            WALL_STRIP_WIDTH,
            wallStripHeight
        );
    }
}

function scaledRect(x, y, width, height) {
    rect(MINIMAP_SCALE_FACTOR * x, MINIMAP_SCALE_FACTOR * y, MINIMAP_SCALE_FACTOR * width, MINIMAP_SCALE_FACTOR * height);
}

function scaledCircle(x, y, radius) {
    circle(MINIMAP_SCALE_FACTOR * x, MINIMAP_SCALE_FACTOR * y, MINIMAP_SCALE_FACTOR * radius);
}

function scaledLine(px, py, qx, qy) {
    line(MINIMAP_SCALE_FACTOR * px, MINIMAP_SCALE_FACTOR * py, MINIMAP_SCALE_FACTOR * qx, MINIMAP_SCALE_FACTOR * qy);
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
    clear("#212121");
    update();

    renderProjectedWalls();
    grid.render();
    for (ray of rays) {
        ray.render();
    }
    player.render();
}
