description('Various tests for .willValidate property');

var parent = document.createElement('div');
document.body.appendChild(parent);

debug('Existence of .willValidate');
parent.innerHTML = '<form>'
    + '<input name="victim"/>'
    + '<textarea name="victim"></textarea>'
    + '<fieldset name="victim">Test</fieldset>'
    + '<button name="victim">'
    + '<select name="victim"></select>'
    + '</form>';
var controls = document.getElementsByName('victim');
    for (var i = 0; i < controls.length; i++)
        shouldBe('typeof controls[i].willValidate', '"boolean"');

debug('');
debug('Form association');
parent.innerHTML = '<input name="test">';
var input = document.getElementsByTagName("input")[0];
shouldBeTrue('input.willValidate');
parent.innerHTML = '<form><input name="test"></form>';
input = document.getElementsByTagName("input")[0];
shouldBeTrue('input.willValidate');

debug('');
debug('Control name');
parent.innerHTML = '<form><input></form>';
input = document.getElementsByTagName("input")[0];
shouldBeTrue('input.willValidate');
parent.innerHTML = '<form><input name="test"></form>';
input = document.getElementsByTagName("input")[0];
shouldBeTrue('input.willValidate');

debug('');
debug('Disabled control');
parent.innerHTML = '<form><input name="test" disabled></form>';
input = document.getElementsByTagName("input")[0];
shouldBeFalse('input.willValidate');

debug('');
debug('Read-only control');
parent.innerHTML = '<form><input name="test" readonly></form>';
input = document.getElementsByTagName("input")[0];
shouldBeFalse('input.willValidate');

debug('');
debug('Input types');
parent.innerHTML = '<form><input name="test"></form>';
input = document.getElementsByTagName("input")[0];
shouldBeTrue('input.willValidate');
shouldBeFalse('input.type = "button"; input.willValidate');
shouldBeFalse('input.type = "submit"; input.willValidate');
shouldBeFalse('input.type = "hidden"; input.willValidate');
shouldBeFalse('input.type = "reset"; input.willValidate');

debug('');
debug('Fieldset element');
parent.innerHTML = '<form><fieldset><p>Fieldset test</p></fieldtset></form>';
shouldBeFalse('document.getElementsByTagName("fieldset")[0].willValidate');

debug('');
debug('Textarea element');
parent.innerHTML = '<form><textarea name="text"></textarea></form>';
shouldBeTrue('document.getElementsByTagName("textarea")[0].willValidate');

var successfullyParsed = true;
