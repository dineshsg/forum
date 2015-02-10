<?php
    
    function forum_form_user_profile_form_alter(&$form,$form_state,$form_id){
        
        global $user;
          if ($form_id === 'user_profile_form' ) 
          { 
                #$form['account']['mail']['#disabled'] = true;
                $result_grade = db_query('SELECT id,grade_name FROM forum_grades') ->fetchAll();
                $grd_nm = $result_grade[0]->grade_name;
                $grd_id = $result_grade[0]->id;
                $option[''] = '----Select----';        
                foreach ($result_grade as $node) {
                    $option[$node->id] = $node->grade_name;
                }
                
                $form['grades_name'] = array(
			        '#type' => 'textfield',
			        '#size' => 60,
			        '#title' => t('Add Grades'),
			        '#required' => TRUE,
			        '#description' => "Please add grades.",
		        );
                $form['grade'] = array(
                    '#title' => t('Grade'),
                    '#type' => 'select',
                    '#options' => $option,
                    '#attributes' => array('class' => array('grade')),
                    '#validated' => TRUE,
                );
                $form['mobile'] = array(
			        '#type' => 'textfield',
			        '#size' => 60,
			        '#title' => t('Mobile No'),
			        '#required' => TRUE,
			        '#description' => "Please enter your mobile number.",
		        );

                $form['subject'] = array(
                    '#title' => t('Subject'),
                    '#type' => 'select',
                    '#options' => array(''=> '-- Select --'),
                    '#attributes' => array('class' => array('subject')),
                    '#validated' => TRUE
                );
                return $form;                
          }
    }
?>
