class Animal {
  constructor() {
    this.name = "boo";
  }

  say() {
    console.log("moo");
  }
}

const monkey = new Animal();

console.log(monkey)
monkey.say();
