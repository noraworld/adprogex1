alert("hello world");

var count = 100;
document.onkeydown = function(e) {
    if (e.keyCode == 32) {
	var hello = document.getElementById("hello");
	hello.style.position = 'absolute';
	hello.style.left = count + "px";
	hello.style.top = count + "px";
	count = count + 100;
	if (count >= 600)
	    count = 100;
   }
}
