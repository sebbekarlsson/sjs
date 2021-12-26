class Vec {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }

  add(other){
    return new Vec(this.x + other.x, this.y + other.y);
  }
}

const myvec = new Vec(16, 16);

const result = myvec.add(new Vec(120, 120));

console.log(result);
