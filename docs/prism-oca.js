Prism.languages.oca = {
	'comment': {
		pattern: /(^|[^\\])#.*/,
		lookbehind: true
	},
	'string': {
		pattern: /("|')(?:\\.|(?!\1)[^\\\r\n])*\1/,
		greedy: true
	},
	'keyword': /\b(do|with|if|then|else|return|break|yield|pub)\b/,
	'builtin':/\b(println|print|debug|input|pause|assert|error|type|abs|acos|asin|atan|acot|cos|sin|tan|cot|max|min|rad|deg|pi|log|ln|lg|random|seed|sqrt|cbrt|read|write|date|clock|execute)|\.(times|ascii|floor|ceil|round|size|int|real|upcase|lowcase|each|find|replace|at|size|insert|remove|at|sort)\b/,
	'boolean': /\b(true|false)\b/,
	'number': /\b((0b[01]+)|(0x[0-9A-Fa-f]+)|([0-9]+(\.[0-9]+)?[eE]-?[0-9]+(\.[0-9]+)?)|(-?[0-9]+(\.[0-9]+)?))/i,
	'operator': /(\+|-|\*|\/|%|\^|<|>|==|<=|>=|!=|\.\.|and|or|xor|lsh|rsh)/,
	'punctuation': /[{}[\];(),.:]/
};
