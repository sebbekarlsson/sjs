# Benchmarks (updated at 2021-10-31 14:14:11.518510)

## Interpreter




    
### object_assign.js
<details>
<summary>Show code</summary>


``` javascript
const person = {
  name: "john",
  age: 33
};

console.log(person.age);
person.age = 1;
console.log(person.age);

```


</details>


#### Runs

* command: `sjs sourcefiles/object_assign.js`
* time: 0:00:00.001268

* command: `node sourcefiles/object_assign.js`
* time: 0:00:00.021044



    
### parseInt.js
<details>
<summary>Show code</summary>


``` javascript
const value = "1337";

const intvalue = parseInt(value, 10);

console.log(intvalue);

```


</details>


#### Runs

* command: `sjs sourcefiles/parseInt.js`
* time: 0:00:00.001178

* command: `node sourcefiles/parseInt.js`
* time: 0:00:00.021629



    
### function_add.js
<details>
<summary>Show code</summary>


``` javascript
function add(x, y) {
  return (x + y) / 2;
}

const r = add(20, 33);

console.log(r);

```


</details>


#### Runs

* command: `sjs sourcefiles/function_add.js`
* time: 0:00:00.001151

* command: `node sourcefiles/function_add.js`
* time: 0:00:00.020378



    
### string_map.js
<details>
<summary>Show code</summary>


``` javascript
const name = "here is a string with some words";

const namearr = Array.from(name);
const mapped = namearr.map((g) => 1);
console.log(mapped);

```


</details>


#### Runs

* command: `sjs sourcefiles/string_map.js`
* time: 0:00:00.002305

* command: `node sourcefiles/string_map.js`
* time: 0:00:00.022148



    
### string_concat.js
<details>
<summary>Show code</summary>


``` javascript
const firstname = "john";
const lastname = "doe";

const fullname = firstname + " " + lastname;

console.log(fullname);

```


</details>


#### Runs

* command: `sjs sourcefiles/string_concat.js`
* time: 0:00:00.001365

* command: `node sourcefiles/string_concat.js`
* time: 0:00:00.021324



    
### array_from.js
<details>
<summary>Show code</summary>


``` javascript
const mystring = "hello world";
console.log(Array.from(mystring));

```


</details>


#### Runs

* command: `sjs sourcefiles/array_from.js`
* time: 0:00:00.001507

* command: `node sourcefiles/array_from.js`
* time: 0:00:00.021843



