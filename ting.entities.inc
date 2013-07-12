<?php

/**
 * @file
 * Entity classes for ting objects and collections.
 */

/**
 * Object representing a relation between TingEntites.
 */
class TingRelation extends DingEntityBase {

  public $type = DingEntityBase::NULL;

  public $uri = DingEntityBase::NULL;

  public $object = DingEntityBase::NULL;

  public function __construct($type, $uri, $object = NULL) {
    parent::__construct();
    $this->properties['type'] = $type;
    $this->properties['uri'] = $uri;
    if ($object) {
      $this->properties['object'] = $object;
    }
  }

  public function getObject() {
    return $this->properties['object'];
  }

}

/**
 * Ting object entity.
 */
class TingEntity extends DingEntity {

  protected $type = DingEntityBase::NULL;

  protected $serieNumber = DingEntityBase::NULL;

  protected $serieTitle = DingEntityBase::NULL;

  protected $record = DingEntityBase::NULL;

  protected $relations = DingEntityBase::NULL;

  protected $localId = DingEntityBase::NULL;
  protected $_localId;

  protected $ownerId = DingEntityBase::NULL;

  protected $creators = DingEntityBase::NULL;
  protected $_creators = array();

  protected $date = DingEntityBase::NULL;
  protected $_date;

  protected $subjects = DingEntityBase::NULL;

  protected $language = DingEntityBase::NULL;

  protected $onlineUrl = DingEntityBase::NULL;

  protected $acSource = DingEntityBase::NULL;

  protected $description = DingEntityBase::NULL;

  protected $contributors = DingEntityBase::NULL;

  protected $isPartOf = DingEntityBase::NULL;

  protected $extent = DingEntityBase::NULL;

  protected $classification = DingEntityBase::NULL;

  protected $isbn = DingEntityBase::NULL;

  protected $descriptionSeries = DingEntityBase::NULL;

  protected $image = array();

  public function getExtent() {
    $extent = $this->data->get('extent');
    return !empty($extent) ? $extent : FALSE;
  }

  public function getClassification() {
    $dk5 = $this->data->get('subject_dk5');
    if ($dk5 == 'sk') {
      $dk5 = '';
    }
    return $dk5;
  }

  public function getSubjectDK5Text() {
    $result = array();
    $dk5 = $this->data->get('subject_dk5text');
    if (!empty($dk5)) {
      if (!is_array($dk5)) {
        $dk5 = array($dk5);
      }
      $result = $dk5;
    }
    return $result;
  }

  public function getSubjectDBCS() {
    $result = array();
    $subjects = $this->data->get('subject_dbcs');
    if (!empty($subjects)) {
      if (!is_array($subjects)) {
        $subjects = array($subjects);
      }
      $result = $subjects;
    }
    return $result;
  }

  public function getIsPartOf() {
    $isPartOf = $this->data->get('ispartof');
    return !empty($isPartOf) ? $isPartOf : FALSE;
  }

  // pjo 26-09-11 ; getmethod contributor
  public function getContributors() {
    $contributors = $this->data->get('contributor');
    return $contributors;
  }

  public function getLocalId() {
    if (empty($this->_localId)) {
      $id = explode(':', $this->data->getObjectId());
      $this->_localId = $id[1];
    }
    return $this->_localId;
  }

  public function setLocalId($id) {
    $this->_localId = $id;
  }

  public function setDingEntityId($id) {
    $this->ding_entity_id = $id;
  }

  public function getOwnerId() {
    $owner = $this->data->get('ownerid');
    return !empty($owner) ? $owner : FALSE;
  }

  public function getType() {
    if (empty($this->_type)) {
      $this->_type = $this->data->get('type_bibdktype');
    }

    return $this->_type;
  }

  public function setType($type) {
    $this->_type = $type;
  }

  public function getTitle() {
    if (empty($this->_title)) {
      $title = $this->getOneOf('title_full', 'title');
      $this->_title = $title;
    }
    return $this->_title;
  }

  public function setTitle($title) {
    $this->_title = $title;
  }

  function getYear() {
    $year = $this->data->get('date');

    return !empty($year) ? $year : FALSE;
  }

  public function getSerieNumber() {
    $serie = $this->splitSerie();
    // Last part is the number.
    if (empty($serie[1])) {
      return FALSE;
    }
    return $serie[1];
  }

  private function splitSerie() {
    $serie = $this->data->get('title_series');
    return explode(';', (array) $serie);
  }

  public function getSerieTitle() {
    $serie = $this->splitSerie();
    // First part is the title.
    if (empty($serie[0])) {
      return FALSE;
    }
    return trim($serie[0]);
  }

  public function getAbstract() {
    $abstract = $this->data->get('abstract');
    return !empty($abstract) ? $abstract : FALSE;
  }

  public function getRelations() {
    $relations = $this->data->getRelations();

    $this->relations = array();
    $relation_objects = array();
    $entity_ids = array();

    foreach ($relations as $relation) {
      if (!empty($relation['object'])) {
        $entity_ids[] = $relation['uri'];
      }
    }
    if (!empty($entity_ids)) {
      $objects = entity_load('ting_object', array(), array('ding_entity_id' => $entity_ids));
      foreach ($objects as $object) {
        $relation_objects[$object->id] = $object;
      }
    }

    foreach ($relations as $relation) {
      $this->relations[] = new TingRelation($relation['type'], $relation['uri'], isset($relation_objects[$relation['uri']]) ? $relation_objects[$relation['uri']] : NULL);
    }

    return $this->relations;
  }

  public function getCreators() {
    if (empty($this->_creators)) {
      $creators = $this->getOneOf('creator', 'creator_aut');
      if (empty($creators)) {
        $this->_creators = array();
      }
      elseif (!empty($creators) && !is_array($creators)) {
        $creators = array($creators);
      }
      $this->_creators = $creators;
    }

    return (array) $this->_creators;
  }

  public function setCreators(array $creators) {
    $this->_creators = $creators;
  }

  public function getCreatorMus() {
    return $this->data->get('creator_mus');
  }

  public function getSubjects() {
    $subjects = $this->data->get('subject');
    if (!empty($subjects) && !is_array($subjects)) {
      $subjects = array($subjects);
    }
    return $subjects;
  }

  public function getLanguage() {
    $language = $this->data->get('language');
    return !empty($language) ? $language : FALSE;
  }

  public function getLanguageIso() {
    $language = $this->data->get('language_iso639_2');
    return !empty($language) ? $language : FALSE;
  }

  public function getDate() {
    if (empty($this->_date)) {
      $this->_date = $this->data->get('date');
    }
    return $this->_date;
  }

  public function setDate($date) {
    $this->_date = $date;
  }

  public function getSubjectGenre() {
    $date = $this->data->get('subject_genre');
    return !empty($date) ? $date : FALSE;
  }

  public function getOnlineUrl() {
    $url = $this->data->get('identifier_uri');
    if (!empty($url)) {
      drupal_alter('ting_online_url', $url, $this);
      return $url;
    }
  }

  public function getSource() {
    $source = $this->data->get('source');
    return !empty($source) ? $source : FALSE;
  }

  public function getAcSource() {
    $source = $this->data->get('ac_source');
    return !empty($source) ? $source : FALSE;
  }

  public function getDescription() {
    $description = $this->data->get('description');
    return !empty($description) ? $description : FALSE;
  }

  /**
   * Get ISBN numbers of the object.
   *
   * @return array
   */
  public function getIsbn() {
    $isbn = NULL;

    if (isset($this->data)) {
      $isbn = $this->data->get('identifier_isbn');
    }

    // Nothing to do.
    if (empty($isbn)) {
      return array();
    }

    if (!is_array($isbn)) {
      $isbn = array($isbn);
    }

    // Get ISBN numbers.
    foreach ($isbn as $k => $number) {
      $isbn[$k] = str_replace(array(' ', '-'), '', $number);
    }
    rsort($isbn);
    return $isbn;
  }

  public function getIssn() {
    return $this->data->get('identifier_issn');
  }

  public function getDescriptionSeries() {
    return $this->data->get('description_series');
  }

  public function getFormat() {
    return $this->data->get('format');
  }

  public function getLanguageSpoken() {
    return $this->data->get('language_spoken');
  }

  public function getLanguageSubtitles() {
    return $this->data->get('language_subtitles');
  }

  public function getSpatial() {
    return $this->data->get('spatial');
  }

  public function getHaspartTrack() {
    return $this->data->get('haspart_track');
  }

  public function getIsreferencedby() {
    return $this->data->get('isreferencedby');
  }

  public function getVersion() {
    return $this->data->get('version');
  }

  public function getPublisher() {
    return $this->data->get('publisher');
  }

  public function getRights() {
    return $this->data->get('rights');
  }

  public function getAudience() {
    return $this->data->get('audience');
  }

  public function getAudienceAge() {
    return $this->data->get('audience_age');
  }

  public function getAudiencePegi() {
    return $this->data->get('audience_pegi');
  }

  public function getAudienceMedieraad() {
    return $this->data->get('audience_medieraad');
  }

  public function get($key) {
    return $this->data->get($key);
  }

  public function __get($prop) {
    if (method_exists($this, 'get' . ucfirst($prop))) {
      if (module_exists('devel')) {
        kpr(ddebug_backtrace());
      }
      watchdog('ting', t('Use getter methor for: @prop'), array('@prop' => $prop), WATCHDOG_CRITICAL);
      throw new Exception('Use getter methor for: ' . $prop);
    }
    return parent::__get($prop);
  }

  public function getFormatsAvailable() {
    return $this->data->getFormats();
  }

  public function getIdentifier() {
    return $this->data->get('identifier');
  }

  public function getIdentifierUri() {
    return $this->data->get('identifier_uri');
  }

  /**
   * Set cover image path.
   *
   * @param string $img
   *   Image path.
   * @param string $style
   *   Image style.
   */
  public function setImage($img, $style = '-none-') {
    $this->image[$style] = $img;
  }

  public function getImage($style = '-none-') {
    if (empty($this->image[$style])) {
      return NULL;
    }
    return $this->image[$style];
  }

  /**
   * Get first non-empty value of the fields.
   * Accepts any number of field names as parameters.
   *
   * @return NULL | mixed
   */
  public function getOneOf() {
    $fields = func_get_args();
    $result = NULL;
    foreach ($fields as $field) {
      $value = $this->get($field);
      if (!empty($value)) {
        $result = $value;
        break;
      }
    }
    return $result;
  }

}
