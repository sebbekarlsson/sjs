function Person(firstname, lastname) {
  this.firstname = firstname;
  this.lastname = lastname;
}


const person = new Person("John", "Doe");

console.log(person.firstname);
console.log(person.lastname);
