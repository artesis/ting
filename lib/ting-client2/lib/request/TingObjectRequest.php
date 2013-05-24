<?php

class TingObjectRequest extends TingGenericRequest {

  protected $action = 'getObjectRequest';

  protected $resultClass = 'TingObjectResult';

  public function setId($id) {
    $this->setParameter('identifier', $id);
  }

  public function setAllRelations($value) {
    $this->setParameter('allRelations', $value);
  }

  public function setRelationData($value) {
    $this->setParameter('relationData', $value);
  }

  public function setObjectFormat($format) {
    $this->setParameter('objectFormat', $format);
  }

}
