# Benchmarks (updated at 2021-10-30 20:48:44.225534)




    
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

* command: sjs sourcefiles/string_map.js
* time: 0:00:00.002642

* command: node sourcefiles/string_map.js
* time: 0:00:00.022506


