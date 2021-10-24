const person = {
  "name": "john doe",
  "age": 33
};

console.log(person);

console.log("---")
console.log(person["name"]);
console.log("---")
console.log(person.age);
console.log(person.name);

person.age = 22;
person.name = "apa";
console.log(person.age);
console.log(person.name);
console.log(person["name"]);
