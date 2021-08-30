function searchRides()
{
	document.getElementById('get-board-error').style.opacity = "0"; 
	document.getElementById('get-destination-error').style.opacity = "0"; 
	document.getElementById('get-hbdsdf-error').style.opacity = "0"; 
	document.getElementById('get-time-error').style.opacity = "0"; 

	var boardingPoint = document.getElementById('get-board').value;
	var destination = document.getElementById('get-destination').value;
	var boardingDate = document.getElementById('hbdsdf').value;
	var boardingTime = document.getElementById('get-time').value;

	var flag = 0;

	if (boardingPoint == "")
	{
		document.getElementById('get-board-error').style.opacity = "1";
		flag++;
	}
	if (destination == "")
	{
		document.getElementById('get-destination-error').style.opacity = "1"; 
		flag++;
	}
	if (boardingDate.localeCompare("Date") == 1)
	{
		document.getElementById('get-hbdsdf-error').style.opacity = "1";
		flag++;
	}
	if (boardingTime == "")
	{
		document.getElementById('get-time-error').style.opacity = "1";
		flag++;
	}

	if(flag == 0)
	{
		/* All the fields are valid */
		/* If session is set, go to find rides page with get vars */
	}
}