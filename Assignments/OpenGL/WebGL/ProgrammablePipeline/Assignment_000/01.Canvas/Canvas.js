// onload function
function main()
{
	// get <canvas> element
	var canvas = document.getElementById("AMC");
	if(!canvas)
	{
		console.log("Obtaining Canvas Failed\n");
	}
	else
	{
		console.log("Obtaining Canvas Succeeded\n");
	}
	
	// print canvas width and height on console
	console.log("Canvas Width : "+canvas.width+" And Canvas Height : "+canvas.height);
	
	// get 2D context
	var context=canvas.getContext("2d");
	if(!context)
	{
		console.log("Obtaining 2D Context Failed\n");
	}
	else
	{
		console.log("Obtaining 2D Context Succeeded\n");
	}
	
	// fill canvas with black color
	context.fillStyle="black"; // "#000000"
	context.fillRect(0, 0, canvas.width, canvas.height);
	
	// center the text 
	context.textAlign="center"; //center horizontally
	context.textBaseline="middle"; //center vertically
	
	// text
	var str="Hello World !!!";
	
	// text font
	context.font="48px sans-serif";
	
	// text color
	context.fillStyle="white"; // "#FFFFFF"
	
	// display the text in center
	context.fillText(str, canvas.width/2, canvas.height/2);
}