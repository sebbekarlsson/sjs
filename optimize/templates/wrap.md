# Benchmarks (updated at {{ now }})

{% from 'benchmark.md' import render_mark %}

{% for mark in marks %}
    {{ render_mark(mark) }}
{% endfor %}
