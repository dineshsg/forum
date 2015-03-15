<?php

    function assign_teacher_form($form, &$form_state)
    {
        $teacher = db_query('SELECT u.name,u.mail,u.uid FROM users u,users_roles ur WHERE ur.rid = 4 AND u.uid = ur.uid') ->fetchAll();
        $teacher_nm = $teacher[0]->name;
        $teacher_id = $teacher[0]->uid;
        $option1[''] = '----Select----';        
        foreach ($teacher as $node) {
            $option1[$node->uid] = $node->name;
        }
        
        $result_grade = db_query('SELECT id,grade_name FROM forum_grades') ->fetchAll();
        $grd_nm = $result_grade[0]->grade_name;
        $grd_id = $result_grade[0]->id;
        $option[''] = '----Select----';        
        foreach ($result_grade as $node) {
            $option[$node->id] = $node->grade_name;
        }
        
        $form['teacher_name'] = array(
            '#title' => t('Teacher Name'),
            '#type' => 'select',
            '#options' => $option1,
            '#attributes' => array('class' => array('teacher_name')),
            '#validated' => TRUE,
        );
        $form['grade'] = array(
            '#title' => t('Grade'),
            '#type' => 'select',
            '#options' => $option,
            '#attributes' => array('class' => array('grade')),
            '#validated' => TRUE,
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
?>
