/*

012
345
678

*/

var answer = [];

$(document).ready(function(){
    $('.answer').click(function(e) {
    	var current = $(this).children(".answer-container").attr("name");

    	/* Quiz Number 1 */

    	if (current == "q1a1") {
    		if (answer[0] == 1) {
    			answer[0] = 0;
    		}
    		else {
    			answer[0] = 1;
    		}

    		if (answer[1] == 1) {
    			answer[1] = 0;
    		}

    		if (answer[2] == 1) {
    			answer[2] = 0;
    		}
    	}

    	else if (current == "q1a2") {
    		if (answer[1] == 1) {
    			answer[1] = 0;
    		}
    		else {
    			answer[1] = 1;
    		}

    		if (answer[0] == 1) {
    			answer[0] = 0;
    		}

    		if (answer[2] == 1) {
    			answer[2] = 0;
    		}
    	}

    	else if (current == "q1a3") {
    		if (answer[2] == 1) {
    			answer[2] = 0;
    		}
    		else {
    			answer[2] = 1;
    		}

    		if (answer[0] == 1) {
    			answer[0] = 0;
    		}

    		if (answer[1] == 1) {
    			answer[1] = 0;
    		}
    	}

    	if (answer[0] == 1) {
    		$("[name='q1a1']").addClass("selected-answer");
    		$("[name='q1a2']").removeClass("selected-answer");
    		$("[name='q1a3']").removeClass("selected-answer");
    	}

    	else if (answer[1] == 1) {
    		$("[name='q1a2']").addClass("selected-answer");
    		$("[name='q1a1']").removeClass("selected-answer");
    		$("[name='q1a3']").removeClass("selected-answer");
    	}

    	else if (answer[2] == 1) {
    		$("[name='q1a3']").addClass("selected-answer");
    		$("[name='q1a1']").removeClass("selected-answer");
    		$("[name='q1a2']").removeClass("selected-answer");
    	}

    	else {
    		$("[name='q1a1']").removeClass("selected-answer");
    		$("[name='q1a2']").removeClass("selected-answer");
    		$("[name='q1a3']").removeClass("selected-answer");
    	}

    	if (answer[0] || answer[1] || answer[2]) {
    		$(".quiz-1-next").addClass("quiz-next-enabled");
    	}

    	else {
    		$(".quiz-1-next").removeClass("quiz-next-enabled");
    	}

    	/* Quiz Number 2 */

    	if (current == "q2a1") {
    		if (answer[3] == 1) {
    			answer[3] = 0;
    		}
    		else {
    			answer[3] = 1;
    		}

    		if (answer[4] == 1) {
    			answer[4] = 0;
    		}

    		if (answer[5] == 1) {
    			answer[5] = 0;
    		}
    	}

    	else if (current == "q2a2") {
    		if (answer[4] == 1) {
    			answer[4] = 0;
    		}
    		else {
    			answer[4] = 1;
    		}

    		if (answer[3] == 1) {
    			answer[3] = 0;
    		}

    		if (answer[5] == 1) {
    			answer[5] = 0;
    		}
    	}

    	else if (current == "q2a3") {
    		if (answer[5] == 1) {
    			answer[5] = 0;
    		}
    		else {
    			answer[5] = 1;
    		}

    		if (answer[3] == 1) {
    			answer[3] = 0;
    		}

    		if (answer[4] == 1) {
    			answer[4] = 0;
    		}
    	}

    	if (answer[3] == 1) {
    		$("[name='q2a1']").addClass("selected-answer");
    		$("[name='q2a2']").removeClass("selected-answer");
    		$("[name='q2a3']").removeClass("selected-answer");
    	}

    	else if (answer[4] == 1) {
    		$("[name='q2a2']").addClass("selected-answer");
    		$("[name='q2a1']").removeClass("selected-answer");
    		$("[name='q2a3']").removeClass("selected-answer");
    	}

    	else if (answer[5] == 1) {
    		$("[name='q2a3']").addClass("selected-answer");
    		$("[name='q2a1']").removeClass("selected-answer");
    		$("[name='q2a2']").removeClass("selected-answer");
    	}

    	else {
    		$("[name='q2a1']").removeClass("selected-answer");
    		$("[name='q2a2']").removeClass("selected-answer");
    		$("[name='q2a3']").removeClass("selected-answer");
    	}

    	if (answer[3] || answer[4] || answer[5]) {
    		$(".quiz-2-next").addClass("quiz-next-enabled");
    	}

    	else {
    		$(".quiz-2-next").removeClass("quiz-next-enabled");
    	}

    	/* Quiz Number 3 */

    	if (current == "q3a1") {
    		if (answer[6] == 1) {
    			answer[6] = 0;
    		}
    		else {
    			answer[6] = 1;
    		}

    		if (answer[7] == 1) {
    			answer[7] = 0;
    		}

    		if (answer[8] == 1) {
    			answer[8] = 0;
    		}
    	}

    	else if (current == "q3a2") {
    		if (answer[7] == 1) {
    			answer[7] = 0;
    		}
    		else {
    			answer[7] = 1;
    		}

    		if (answer[6] == 1) {
    			answer[6] = 0;
    		}

    		if (answer[8] == 1) {
    			answer[8] = 0;
    		}
    	}

    	else if (current == "q3a3") {
    		if (answer[8] == 1) {
    			answer[8] = 0;
    		}
    		else {
    			answer[8] = 1;
    		}

    		if (answer[6] == 1) {
    			answer[6] = 0;
    		}

    		if (answer[7] == 1) {
    			answer[7] = 0;
    		}
    	}

    	if (answer[6] == 1) {
    		$("[name='q3a1']").addClass("selected-answer");
    		$("[name='q3a2']").removeClass("selected-answer");
    		$("[name='q3a3']").removeClass("selected-answer");
    	}

    	else if (answer[7] == 1) {
    		$("[name='q3a2']").addClass("selected-answer");
    		$("[name='q3a1']").removeClass("selected-answer");
    		$("[name='q3a3']").removeClass("selected-answer");
    	}

    	else if (answer[8] == 1) {
    		$("[name='q3a3']").addClass("selected-answer");
    		$("[name='q3a1']").removeClass("selected-answer");
    		$("[name='q3a2']").removeClass("selected-answer");
    	}

    	else {
    		$("[name='q3a1']").removeClass("selected-answer");
    		$("[name='q3a2']").removeClass("selected-answer");
    		$("[name='q3a3']").removeClass("selected-answer");
    	}

    	if (answer[6] || answer[7] || answer[8]) {
    		$(".quiz-3-next").addClass("quiz-next-enabled");
    	}

    	else {
    		$(".quiz-3-next").removeClass("quiz-next-enabled");
    	}

    });
});