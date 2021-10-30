{% macro render_mark(mark) %}
### {{ mark.filename }}
<details>
<summary>Show code</summary>


``` javascript
{{ mark.code }}
```


</details>


#### Runs
{% for run in mark.runs %}
* command: `{{ run.cmd }}`
* time: {{ run.time }}
{% endfor %}
{% endmacro %}
