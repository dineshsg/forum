<?php
    function forum_search($form, &$form_state)
    {
        $form = array();
        $form['tfname'] = array(
		    '#type' => 'textfield',
		    '#size' => 30,
		    '#title' => t('First Name'),
		    '#required' => TRUE,
		    '#default_value' => $fname,
		    '#description' => "Please enter your First Name.",
		);
		$form["submit"] = array(
            '#type' => 'submit',
            '#value' => "Search"
        );
    }
?>
