<?php
    function forum_question_search($form, &$form_state)
    {
        $form = array();		
        $form['search'] = array(
		    '#type' => 'textfield',
		    '#size' => 30,
		    '#required' => TRUE,
		);
		$form["submit"] = array(
            '#type' => 'submit',
            '#value' => "Search",
        );
        return $form;
    }
?>
