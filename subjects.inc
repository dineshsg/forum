<?php

    function subjects_settings_form($form_state)
    {
        $form['#attributes'] = array('enctype' => "multipart/form-data", 'class' => 'addsubjects');
        
        $result_grade = db_query('SELECT id,grade_name FROM forum_grades') ->fetchAll();
        $grd_nm = $result_grade[0]->grade_name;
        $grd_id = $result_grade[0]->id;
        
        foreach ($result_grade as $node) {
            //$option[] = array("$node->id" => t("$node->grade_name"));
            $option[$node->id] = $node->grade_name;
        }
        $form['grade'] = array(
            '#title' => t('Grade'),
            '#type' => 'select',
            '#options' => $option,
        );

		$form['subject_name'] = array(
			'#type' => 'textfield',
			'#title' => t('Add Subjects'),
			'#required' => TRUE,
			'#description' => "Please add grades.",
		);

		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Submit')
	   );
	   
	   return $form;
   }
   
   function subjects_settings_form_submit($form, &$form_state)
   {
        global $user;
        $query = "INSERT INTO `forum_subjects` (`subject_name`, `grade_id` ,`user_id`, `created_date`, `updated_date`) VALUES ('".$form_state['values']['subject_name']."', '".$form_state['values']['grade']."' , '".$user->uid."', (now()), (now()))";

        if(db_query($query)){
			drupal_set_message('New Subject Added Successfully');
		}else {
			drupal_set_message('Error while Adding Subject , Please try again.');
		}
		
    }

    function _list_subjects()
    {
        if (!isset($output)) $output='';
        
        $form = drupal_get_form('subjects_settings_form');
        $output .= drupal_render($form);
        
        $header = array(
            array('data'=>'Subject', 'field'=>'subject_name', 'sort'=>'asc'),
            array('data'=>'Grade', 'field'=>'grade_name', 'sort'=>'asc'),
		    array('data'=>'User Name', 'field'=>'name'),
		    array('data'=>'Operations', 'colspan' => 2),
    	);
    	
        $results = db_query('SELECT fs.subject_name,fg.grade_name,u.name,fs.id FROM forum_subjects fs, users u, forum_grades fg WHERE fs.grade_id = fg.id AND fs.user_id = u.uid');
        
        foreach ($results as $node) {
            $rows[] = array(
                'subject_name' => $node->subject_name,
                'grade_name' => $node->grade_name,
                'name' => $node->name,
                'modify' => l('Modify', 'list/subject/modify/'.$node->id, array('html' => TRUE)),
                'delete' => l('Delete', 'list/subject/delete/' . $node->id, array('html' => TRUE)),
            );
        }
        
        if(isset($rows))
        {
            $output .= theme('table', array('header' => $header,'rows' => $rows ));
        }
        else
        {
            $output .= "<br><h3>No Data</h3><br>";
        }
        return $output;

    }
   
    function subject_modify_form($form, &$form_state) {

        $subject_id = arg(3);
        $result_subject = db_query('SELECT fs.subject_name,fg.grade_name,fs.grade_id FROM forum_subjects fs, forum_grades fg WHERE fs.id = :nid AND fs.grade_id = fg.id',array(':nid' => $subject_id))->fetchAll();
        $result_subject1 = $result_subject[0]->subject_name;
        $result_subject2 = $result_subject[0]->grade_id;

        $result_grade = db_query('SELECT id,grade_name FROM forum_grades')->fetchAll();
        $grd_nm = $result_grade[0]->grade_name;                                 
        $grd_id = $result_grade[0]->id;                                         
                                                                                       
        foreach ($result_grade as $node) {                                      
        //$option[] = array("$node->id" => t("$node->grade_name"));         
            $option[$node->id] = $node->grade_name;                             
        }                                                                       
        $form['grade'] = array(                                                 
            '#title' => t('Grade'),                                             
            '#type' => 'select',                                                
            '#options' => $option,
            '#default_value' => $result_subject2,            
        ); 
        $form['subjects_name_update'] = array(
			    '#type' => 'textfield',
			    '#title' => t('Add Grades'),
			    '#required' => TRUE,
			    '#description' => "Please add grades.",
			    '#value' => $result_subject1,
	    ); 
	    $form['submit'] = array(
	            '#type' => 'submit',
			    '#value' => t('Submit')
	    );
        return $form;
    }

    function subject_modify_form_submit($form, &$form_state)
    {
        global $user;
        
        $cur_time = $mydate = date('Y-m-d', strtotime(date('d/m/Y')));
        $query = "UPDATE `forum_subjects` SET `subject_name`='".$_POST['subjects_name_update']."', `user_id` = '".$user->uid."', `updated_date` = '".$cur_time."' WHERE id = '".arg(3)."'";
        
        if(db_query($query)){
            drupal_set_message('New Grade Category Added Successfully');
	    }else {
	        drupal_set_message('Error while Adding Grade Category, Please try again.');
	    }
	    
	    $form_state['redirect'] = 'admin/list_subjects';  
    }

    function subject_delete_confirm($form, &$form_state)
    {
        $subject_id = arg(3);
        
        $result_subject_del = db_query('SELECT subject_name FROM forum_subjects WHERE id = :nid',array(':nid' => $subject_id)) ->fetchAll();
        
        if($result_subject_del)
        {
            $result_subject_del1 = $result_subject_del['0']->subject_name;
            return confirm_form($form, t('Are you sure you want to delete %subject', array('%subject' => $result_subject_del['0']->subject_name)),'list/subject/delete/'.arg(3),t('This action cannot be undone'),t('Delete'),t('Cancel'));
        }
        else
        {
            drupal_goto('admin/list_subjects');
            drupal_set_message('The requested subject does not exist!');
        }
    
    }
    function subject_delete_confirm_submit($form, $form_state)
    {
        if($form_state['values']['confirm'])
        {
            $grade_id = arg(3);
            $result = "DELETE FROM forum_subjects WHERE id='".$grade_id."'";
            
            if(db_query($result))
            {
                drupal_set_message('Subject has been sucessfully deleted');
                
            }
            else
            {
                drupal_set_message('Error while Deleting Subject Category, Please try again.');
            }
            
        }
        drupal_goto('admin/list_subjects');  
    }
?>
