const person = {
  name: "john",
  age: 33,
  pet: {
    type: "dog",
    age: 2
  }
};

console.log(person.pet.age);
person.pet.age = 5;
console.log(person.pet.age);
